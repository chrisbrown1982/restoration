// C++ code to perform add, multiply and transpose on sparse matrices. Adapted
// lazily from https://www.geeksforgeeks.org/operations-sparse-matrices/ (no
// dynamic memory deallocation, etc.). Made floating-point for comparison with
// SEQ_mm.

#include <iostream>

// Maximum number of elements in matrix
static const int MAX = 100;

using namespace std;

class element {
public:
int row;
int col;
float value;
};

class sparse_matrix {

public:

  // Array representation
  // of sparse matrix
  element data[MAX];

  // dimensions of matrix
  int row, col;

  // total number of elements in matrix
  int len;

  sparse_matrix(int r, int c) {

    // initialize row
    row = r;

    // initialize col
    col = c;

    // intialize length to 0
    len = 0;
  }

  // insert elements into sparse matrix
  void insert(int r, int c, float val) {

    // invalid entry
    if (r > row || c > col) {
      cerr << "Wrong entry" << endl;
    }

    else {

      // insert row value
      data[len].row = r;

      // insert col value
      data[len].col = c;

      // insert element's value
      data[len].value = val;

      // increment number of data in matrix
      len++;
    }
  }

  sparse_matrix * transpose() {

    // new matrix with inversed row X col
    sparse_matrix * result = new sparse_matrix(col, row);

    // same number of elements
    result->len = len;

    // to count number of elements in each column
    int * count = new int[col + 1];

    // initialize all to 0
    for (int i = 1; i <= col; i++)
      count[i] = 0;

    for (int i = 0; i < len; i++)
      count[data[i].col]++;

    int * index = new int[col + 1];

    // to count number of elements having col smaller
    // than particular i

    // as there is no col with value < 1
    index[1] = 0;

    // initialize rest of the indices
    for (int i = 2; i <= col; i++)

      index[i] = index[i - 1] + count[i - 1];

    for (int i = 0; i < len; i++) {

      // insert a data at rpos and increment its value
      int rpos = index[data[i].col]++;

      // transpose row=col
      result->data[rpos].row = data[i].col;

      // transpose col=row
      result->data[rpos].col = data[i].row;

      // same value
      result->data[rpos].value = data[i].value;
    }

    // the above method ensures
    // sorting of transpose matrix
    // according to row-col value
    return result;
  }

  void multiply(sparse_matrix * b) {

    if (col != b->row) {

      // Invalid multiplication
      cerr << "Can't multiply, Invalid dimensions" << endl;

      return;
    }

    // transpose b to compare row
    // and col values and to add them at the end
    b = b->transpose();
    int apos, bpos;

    // result matrix of dimension row X b->col
    // however b has been transposed, hence row X b->row
    sparse_matrix * result = new sparse_matrix(row, b->row);


    // iterate over all elements of A
    for (apos = 0; apos < len;) {

      // current row of result matrix
      int r = data[apos].row;

      // iterate over all elements of B
      for (bpos = 0; bpos < b->len;) {

        // current column of result matrix
        // data[][0] used as b is transposed
        int c = b->data[bpos].row;

        // temporary pointers created to add all
        // multiplied values to obtain current
        // element of result matrix
        int tempa = apos;
        int tempb = bpos;

        float sum = 0.0;

        // iterate over all elements with
        // same row and col value
        // to calculate result[r]
        while (tempa < len && data[tempa].row == r
               && tempb < b->len && b->data[tempb].row == c) {

          if (data[tempa].col < b->data[tempb].col)

            // skip a
            tempa++;

          else if (data[tempa].col > b->data[tempb].col)

            // skip b
            tempb++;
          else

            // same col, so multiply and increment
            sum += data[tempa++].value * b->data[tempb++].value;
        }

        // insert sum obtained in result[r]
        // if its not equal to 0
        if (sum != 0.0)
          result->insert(r, c, sum);

        while (bpos < b->len && b->data[bpos].row == c)

          // jump to next column
          bpos++;
      }

      while (apos < len && data[apos].row == r)

        // jump to next row
        apos++;
    }

    result->print();
  }

  // printing matrix
  void print() {
    cout << "Dimension: " << row << "x" << col << endl;
    cout << "Sparse Matrix:" << endl << "Row Column Value" << endl;

    for (int i = 0; i < len; i++) {

      cout << data[i].row << " " << data[i].col << " " << data[i].value << endl;
    }
  }
};

int main() {

  // Create two sparse 3x3 matrices. The map pattern is irregular in that all
  // runs include 'fcmp' and 'br' from line "comparison" but only some runs
  // include 'fmul' and 'fadd' from line "operation".
  sparse_matrix * a = new sparse_matrix(3, 3);
  sparse_matrix * b = new sparse_matrix(3, 3);

  a->insert(1, 1, 1);
  a->insert(1, 2, 1);
  a->insert(2, 2, 1);
  a->insert(3, 1, 1);
  a->insert(3, 3, 1);

  b->insert(1, 3, 1);
  b->insert(2, 1, 1);
  b->insert(3, 2, 1);
  b->insert(3, 3, 1);

  // Output result
  cout << endl << "Sparse multiplication: " << endl;
  a->multiply(b);

  // Now create two dense 3x3 matrices. The resulting data-flow is equivalent to
  // that of SEQ_mm, and the map pattern is regular.
  sparse_matrix * c = new sparse_matrix(3, 3);
  sparse_matrix * d = new sparse_matrix(3, 3);

  c->insert(1, 1, 1);
  c->insert(1, 2, 1);
  c->insert(1, 3, 1);
  c->insert(2, 1, 1);
  c->insert(2, 2, 1);
  c->insert(2, 3, 1);
  c->insert(3, 1, 1);
  c->insert(3, 2, 1);
  c->insert(3, 3, 1);

  d->insert(1, 1, 1);
  d->insert(1, 2, 1);
  d->insert(1, 3, 1);
  d->insert(2, 1, 1);
  d->insert(2, 2, 1);
  d->insert(2, 3, 1);
  d->insert(3, 1, 1);
  d->insert(3, 2, 1);
  d->insert(3, 3, 1);

  // Output result
  cout << endl << "Dense multiplication: " << endl;
  c->multiply(d);
}
