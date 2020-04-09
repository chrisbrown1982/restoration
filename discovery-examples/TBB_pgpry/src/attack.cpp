/*
 * pgpry - PGP private key recovery
 * Copyright (C) 2010 Jonas Gehring
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * file: attack.cpp
 * Static attack context
 */


#include <cstdlib>
#include <iostream>
#include <fstream>

#include <tbb/tbb.h>
#include <tbb/pipeline.h>

#include "confio.h"
#include "guessers.h"
#include "options.h"
#include "prefixsuffixfilter.h"
#include "regexfilter.h"
#include "sysutils.h"
#include "tester.h"

#include "attack.h"


// Static variables
Attack *Attack::ctx = NULL;


// Signal handler class
class AttackSigHandler : public SysUtils::SigHandler
{
	public:
		AttackSigHandler(Attack *attack)
			: m_attack(attack)
		{
		}

	protected:
		void setup(sigset_t *set)
		{
			sigaddset(set, SIGINT);
		}

		bool handle(int32_t sig)
		{
			if (m_attack->status() == Attack::STATUS_RUNNING) {
				std::cerr << "Interrupt catched, flushing buffers and saving state..." << std::endl;
				m_attack->saveAndAbort();
			}
			return false;
		}

	private:
		Attack *m_attack;
};


// Private constructor
Attack::Attack(const Options &options)
	: m_options(options), m_guessers(nullptr)
{
}

// Performs the brute-force attack
int32_t Attack::run(const Key &key, const Options &options, ConfReader *reader)
{
	if (ctx) {
		delete ctx;
	}
	ctx = new Attack(options);

	ctx->m_key = key;
	ctx->m_status = STATUS_RUNNING;

	// Setup threads
	uint32_t bufferIndex = 0;
	ctx->m_guessers = setupGuessers(options);

	//ctx->m_threads.insert(ctx->m_threads.begin(), ctx->m_guessers.begin(), ctx->m_guessers.end());

	if (options.useRegexFiltering()) {
		//ctx->m_regexFilters = setupRegexFilters(&ctx->m_buffers[bufferIndex], &ctx->m_buffers[bufferIndex+1], options);
		ctx->m_regexFilters = setupRegexFilters(options);
		//ctx->m_threads.insert(ctx->m_threads.begin(), ctx->m_regexFilters.begin(), ctx->m_regexFilters.end());
		//++bufferIndex;
	}
	if (options.usePrefixSuffixFiltering()) {
		//ctx->m_prefixSuffixFilters = setupPrefixSuffixFilters(&ctx->m_buffers[bufferIndex], &ctx->m_buffers[bufferIndex+1], options);
		ctx->m_prefixSuffixFilters = setupPrefixSuffixFilters(options);
		//ctx->m_threads.insert(ctx->m_threads.begin(), ctx->m_prefixSuffixFilters.begin(), ctx->m_prefixSuffixFilters.end());
		//++bufferIndex;
	}

	//ctx->m_testers = setupTesters(key, &ctx->m_buffers[bufferIndex], options);
	ctx->m_testers = setupTesters(key, options);

	//ctx->m_threads.insert(ctx->m_threads.begin(), ctx->m_testers.begin(), ctx->m_testers.end());

//	ctx->m_mutex.lock();

	// Start signal handler
	//SysUtils::SigHandler::block(SIGINT);
	//AttackSigHandler sigHandler(ctx);
	//sigHandler.start();

	// Start threads 
	//for (uint32_t i = 0; i < ctx->m_guessers.size(); i++) {
		if (reader) {
			try {
				ctx->m_guessers->loadState(reader);
			} catch (const std::string &str) {
				std::cerr << "Error loading guesser state: " << str << std::endl;
				return EXIT_FAILURE;
			} catch (const char *str) {
				std::cerr << "Error loading guesser state: " << str << std::endl;
				return EXIT_FAILURE;
			}
		}

	tbb::pipeline pipeline;
	pipeline.add_filter(*(ctx->m_guessers));
  if (ctx->m_regexFilters)
    pipeline.add_filter(*(ctx->m_regexFilters));
  if (ctx->m_prefixSuffixFilters)
    pipeline.add_filter(*(ctx->m_prefixSuffixFilters));
	pipeline.add_filter(*(ctx->m_testers));
	pipeline.run(16);

	//tbb::filter_t <void, Memblock *> f1 (tbb::filter::serial_in_order,
	//		*(ctx->m_guessers));

	//tbb::filter_t <Memblock *, Memblock *> f2 (tbb::filter::parallel,
	//			*(ctx->m_regexFilters));

	//tbb::filter_t <Memblock *, Memblock *> f3 (tbb::filter::parallel,
	//				*(ctx->m_prefixSuffixFilters));

	//tbb::filter_t <Memblock *, void> f4 (tbb::filter::parallel,
	//					TesterFilter(*(ctx->m_testers)));



		//ctx->m_guessers[i]->start(reader != NULL);
//	}
//	for (uint32_t i = 0; i < ctx->m_regexFilters.size(); i++) {
//		ctx->m_regexFilters[i]->start();
//	}
//	for (uint32_t i = 0; i < ctx->m_prefixSuffixFilters.size(); i++) {
//		ctx->m_prefixSuffixFilters[i]->start();
//	}
//	for (uint32_t i = 0; i < ctx->m_testers.size(); i++) {
//		ctx->m_testers[i]->start();
//	}

	// Now all we've got to do is wait
//	ctx->m_condition.wait(&ctx->m_mutex);

//	ctx->m_mutex.unlock();
	if (ctx->m_status & STATUS_EXHAUSTED) {
		ctx->boilOut();
	}

	switch (ctx->m_status) {
		case STATUS_SUCCESS:
			std::cout << "SUCCESS: Found pass phrase: '" << ctx->m_phrase << "'." << std::endl;
			break;
		case STATUS_EXHAUSTED:
			std::cout << "SORRY, the key space is exhausted. The attack failed." << std::endl;
			break;
		case STATUS_ERROR:
			std::cout << "ERROR: " << ctx->m_errString << std::endl;
			break;
		default:
			break;
	}

	return (ctx->m_status == STATUS_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE);
}

// Called by a cracker which has discovered the pass phrase
void Attack::phraseFound(const Memblock &mblock)
{
	ctx->m_mutex.lock();
	ctx->m_status = ((ctx->m_status & ~STATUS_MASK) | STATUS_SUCCESS);
	ctx->m_phrase = mblock;
	ctx->m_condition.wakeAll();
	ctx->m_mutex.unlock();
}

// Called by a guesser if it's out of phrases
void Attack::exhausted()
{
	ctx->m_mutex.lock();
	ctx->m_status = ((ctx->m_status & ~STATUS_MASK) | STATUS_EXHAUSTED);
	ctx->m_condition.wakeAll();
	ctx->m_mutex.unlock();
}

// Called whenever a problematic error appears, e.g. the key uses an unsupported algorithm
void Attack::error(const std::string &errString)
{
	ctx->m_mutex.lock();
	ctx->m_status = ((ctx->m_status & ~STATUS_MASK) | STATUS_ERROR);
	ctx->m_errString = errString;
	ctx->m_condition.wakeAll();
	ctx->m_mutex.unlock();
}

// Saves the current guesser state and aborts the attack
void Attack::saveAndAbort()
{
	// Abort gueesser threads
	//for (uint32_t i = 0; i < ctx->m_guessers->size(); i++) {
  //ctx->m_guessers->abort();
	//	ctx->m_guessers->wait();
	//}

	// TODO: Check for success?
	//ctx->save();

	//ctx->m_mutex.lock();
	//ctx->m_status = ((ctx->m_status & ~STATUS_MASK) | STATUS_ABORTED);
	//ctx->m_condition.wakeAll();
	//ctx->m_mutex.unlock();
}

// Sets up the pass phrases guessers
Guessers::Guesser * Attack::setupGuessers(const Options &options)
{
	//std::vector<Guessers::Guesser *> guessers;
	Guessers::Guesser *g = Guessers::guesser(options.guesser()); //, out);
	if (g) {
		g->setup(options.guesserOptions());
    g->init();
		//guessers.push_back(g);
	} else {
		std::cerr << "ERROR: No such guessing method: " << options.guesser() << std::endl;
	}
	return g;
}

/*
// Sets up the regular expression filters
std::vector<RegexFilter *> Attack::setupRegexFilters(Buffer *in, Buffer *out, const Options &options)
{
	// VJ: Something needs to be done about this
	std::vector<RegexFilter *> filters;
	for (uint32_t i = 0; i < options.numRegexFilters(); i++) {
		RegexFilter *r = new RegexFilter();
		if (!r->readExpressions(options.regexFile())) {
			break;
		}
		filters.push_back(r);
	return filters;
}*/

RegexFilter * Attack::setupRegexFilters(const Options &options)
{
	// VJ: Something needs to be done about this
	RegexFilter *r = new RegexFilter();
	if (!r->readExpressions(options.regexFile()))
		return nullptr;
	else {
		return r;
	}
}

// Sets up the prefix/suffix filters
/*
std::vector<PrefixSuffixFilter *> Attack::setupPrefixSuffixFilters(const Options &options)
{
	std::vector<PrefixSuffixFilter *> filters;
	PrefixSuffixFilter *r = new PrefixSuffixFilter();
	r->setPrefixes(options.prefixes());
	r->setSuffixes(options.suffixes());
	filters.push_back(r);
	return filters;
}*/

PrefixSuffixFilter * Attack::setupPrefixSuffixFilters(const Options &options)
{
	PrefixSuffixFilter *r = new PrefixSuffixFilter();
	r->setPrefixes(options.prefixes());
	r->setSuffixes(options.suffixes());
	return r;
}


// Sets up the pass phrase testers
/*
std::vector<Tester *> Attack::setupTesters(const Key &key, Buffer *in, const Options &options)
{
	std::vector<Tester *> testers;
	for (uint32_t i = 0; i < options.numTesters(); i++) {
		testers.push_back(new Tester(key, in));
	}
	return testers;
}*/

Tester * Attack::setupTesters(const Key &key, const Options &options)
{
	Tester *tester = new Tester(key);
	return tester;
}

// Saves the current attack state
void Attack::save()
{
	std::ofstream out(PGPRY_STATEFILE);
	ConfWriter *writer;
	if (out.is_open()) {
		writer = new ConfWriter(out);
	} else {
		writer = new ConfWriter(std::cout);
	}
	writer->putComment("");
	writer->putComment(PACKAGE_NAME " " PACKAGE_VERSION " - State dump");
	writer->putComment("");
	writer->putComment("Command-line options");
	m_options.save(writer);
	writer->putComment("Guesser state");
	//for (uint32_t i = 0; i < m_guessers.size(); i++) {
		m_guessers->wait();
		//m_guessers[i]->saveState(writer);
	//}
	delete writer;
	out.close();
}

// Boiling out of the current attack (handle all buffered phrases and finish)
void Attack::boilOut()
{
	m_mutex.lock();
	if (m_status & STATUS_BOILING_OUT) {
		m_mutex.unlock();
		return;
	}
	m_status |= STATUS_BOILING_OUT;
	m_mutex.unlock();

	//for (uint32_t i = 0; i < m_guessers.size(); i++) {
		m_guessers->abort();
		m_guessers->wait();
	//}

	uint32_t bufferIndex = 0;
	while (m_buffers[bufferIndex].size() > 0) {
		SysUtils::Thread::msleep(5);
	}

	if (m_options.useRegexFiltering()) {
		//for (uint32_t i = 0; i < m_regexFilters.size(); i++) {
			m_regexFilters->abort();
		//}
		//for (uint32_t i = 0; i < m_buffers[bufferIndex].capacity(); i++) {
			//m_buffers[bufferIndex].put(Memblock());
		//}
		//for (uint32_t i = 0; i < m_regexFilters.size(); i++) {
			m_regexFilters->wait();
		//}

		//++bufferIndex;
		//while (m_buffers[bufferIndex].size() > 0) {
		//	SysUtils::Thread::msleep(5);
	//	}
	}

	if (m_options.usePrefixSuffixFiltering()) {
		//for (uint32_t i = 0; i < m_prefixSuffixFilters.size(); i++) {
			m_prefixSuffixFilters->abort();
		//}
		//for (uint32_t i = 0; i < m_buffers[bufferIndex].capacity(); i++) {
		//	m_buffers[bufferIndex].put(Memblock());
		//}
		//for (uint32_t i = 0; i < m_prefixSuffixFilters.size(); i++) {
			m_prefixSuffixFilters->wait();
		//}

		//++bufferIndex;
		//while (m_buffers[bufferIndex].size() > 0) {
		//	SysUtils::Thread::msleep(5);
		//}
	}

	//for (uint32_t i = 0; i < m_testers.size(); i++) {
		m_testers->abort();
	//}
	//for (uint32_t i = 0; i < m_buffers[bufferIndex].capacity(); i++) {
	//	m_buffers[bufferIndex].put(Memblock());
	//}
	//for (uint32_t i = 0; i < m_testers.size(); i++) {
		m_testers->wait();
	//}

	m_mutex.lock();
	m_status &= ~STATUS_BOILING_OUT;
	m_mutex.unlock();
}
