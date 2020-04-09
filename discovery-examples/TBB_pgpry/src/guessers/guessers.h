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
 * file: guessers.h
 * Guesser thread definition and factory
 */


#ifndef GUESSERS_H_
#define GUESSERS_H_


#include "main.h"

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <tbb/tbb.h>
#include "threads.h"
#include "sysutils.h"

//class Buffer;
class ConfReader;
class ConfWriter;
class Memblock;


namespace Guessers
{

class Guesser : public SysUtils::Thread, public tbb::filter
{
	public:
		//Guesser(Buffer *buffer);
	    Guesser();
		virtual ~Guesser() { }

		void start(bool resume);

		virtual void setup(const std::map<std::string, std::string> &options);
		virtual std::vector<std::pair<std::string, std::string> > options() const;

		virtual void saveState(ConfWriter *writer);
		virtual void loadState(ConfReader *reader);

		void * operator() (void *);

    virtual void init();
	protected:

		//Memblock * operator()(tbb::flow_control &fc);
		void run();

		//virtual void init();
		virtual bool guess(Memblock *m) = 0;

	private:
		//Buffer *m_buffer;
		bool m_resume;
		int32_t n;
		Memblock *p_block;
		SysUtils::Watch watch;
};



Guesser *guesser(const std::string &name); //, Buffer *buffer);
std::vector<std::pair<std::string, std::string> > guessers();

} // namespace Guessers


#endif // GUESSERS_H_
