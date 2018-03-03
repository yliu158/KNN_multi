#include <vector>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stack>
#include <pthread.h>
#include <thread>
#include "Read.cpp"
#include "stack.cpp"
// #include "Variance.cpp"
using namespace std;

class Node {
public:
  Node* left;
  Node* right;
  int index;
  int dimension;
  Node (int index ,int dimension) {
    this->index = index;
    this->dimension = dimension;
  }
};

struct Parameters{
  vector<vector<double> >* data;
  int begin;
  int end;
  int dimension;
};
bool compareFunc(const vector<double>& first, const vector<double>& second);

class MultiThreadKD {
public:
  Stack<thread> s;

  MultiThreadKD(int core_num) {
    for (int i = 0; i < core_num; ++i) {
      thread t;
      pool.push(t);
    }
  }

  void printData(const vector<vector<double> >& data);
  static void * localSort(void* p);
  void prepareTreeHelper(vector<vector<double> >& data, int begin, int end, int dimension);
  void prepareTree(vector<vector<double> >& data);
  Node* buildTreeHelper(const vector<vector<double> >& data, int begin, int end, int dimension);
  Node* buildTree(vector<vector<double> >& data);
  void printTree(const vector<vector<double> >& data, Node* root);
  vector<vector<double> > randomData(int size_of_data, int dimension);
  double euclideanMetric(const vector<double>& first, const vector<double>& second);
  void query(const vector<vector<double> >& data, Node* current, const vector<double>& p, vector<pair<double, Node*> >& knn);
  void checkMT(const vector<vector<double> >& single, const vector<vector<double> >& multi);
};

void MultiThreadKD::printData(const vector<vector<double> >& data) {
  cout << endl;
  for (vector<double> v: data) {
    for (double d: v) {
      cout << left <<setfill(' ') << setw(10) << left << d;
    }
    cout << endl;
  }
  cout << endl;
}

bool compareFunc(const vector<double>& first, const vector<double>& second) {
  assert(first.back() == second.back());
  int dim_index = first.back();
  return first[dim_index] < second[dim_index];
}

void localSort(vector<vector<double> >& data, int begin, int end, int dimension){
  if (begin >= end) return;
  // for ()
}

void * MultiThreadKD::localSort(void* p) {
  struct Parameters * param = (struct Parameters * )p;
  int begin = param->begin;
  int end = param->end;
  if (begin >= end) return NULL;
  vector<vector<double> >* d = param->data;
  int dimension = param->dimension;

  for (int i = begin; i <= end; ++i) {
    d->at(i).back() = dimension;
  }
  sort(d->begin()+begin, d->begin()+end+1, compareFunc);
  int mid = (begin+end)/2;
  pthread_t pid_1;
  struct Parameters * p_1 = (struct Parameters *)malloc(sizeof(struct Parameters));
  p_1->data = d;
  p_1->begin = begin;
  p_1->end = mid-1;
  p_1->dimension = dimension+1;
  pthread_t pid_2;
  struct Parameters * p_2 = (struct Parameters *)malloc(sizeof(struct Parameters));
  p_2->data = d;
  p_2->begin = mid+1;
  p_2->end = end;
  p_2->dimension = dimension+1;
  cout << "PID_1: " << pid_1 << endl;
  cout << "PID_2: " << pid_2 << endl;
  int s_1 = pthread_create(&pid_1, NULL, MultiThreadKD::localSort, (void*)p_1);
  int s_2 = pthread_create(&pid_2, NULL, MultiThreadKD::localSort, (void*)p_2);

  pthread_join(pid_1, NULL);
  pthread_join(pid_2, NULL);
  free(p_1);
  free(p_2);
  return NULL;
}

void MultiThreadKD::prepareTreeHelper(vector<vector<double> >& data, int begin, int end, int dimension) {
  if (begin >= end) return;
  struct Parameters p;
  p.data = &data;
  p.begin = begin;
  p.end = end;
  p.dimension = dimension;
  MultiThreadKD::localSort(&p);
  int mid = (begin+end)/2;
  prepareTreeHelper(data, begin, mid-1, dimension+1);
  prepareTreeHelper(data, mid+1, end, dimension+1);
  return;
}

void MultiThreadKD::prepareTree(vector<vector<double> >& data) {
  for (int i = 0; i < data.size(); ++i) {
    data[i].push_back(0);
  }
  struct Parameters p;
  p.data = &data;
  p.begin = 0;
  p.end = 0;
  p.dimension = 0;
  pthread_t t;
  pthread_create(&t, NULL, MultiThreadKD::localSort, (void*)&p);
  // localSort(&p);
  pthread_join(t, NULL);
  for (int i = 0; i < data.size(); ++i) {
    data[i].pop_back();
  }
  return;
}

Node* MultiThreadKD::buildTreeHelper(const vector<vector<double> >& data, int begin, int end, int dimension) {
  if (begin > end) return NULL;
  if (begin == end) {
    return new Node(begin, dimension);
  }
  int mid = (begin+end)/2;
  Node* cur = new Node(mid, dimension);
  cur->left = buildTreeHelper(data, begin, mid-1, dimension+1);
  cur->right = buildTreeHelper(data, mid+1, end, dimension+1);
  return cur;
}

Node* MultiThreadKD::buildTree(vector<vector<double> >& data) {
  prepareTree(data);
  Node* root = buildTreeHelper(data, 0, data.size()-1,0);
  return root;
}

void MultiThreadKD::printTree(const vector<vector<double> >& data, Node* root) {
  if (root == NULL) return;
  printTree(data, root->left);
  for (int i = 0; i < root->dimension; ++i) {
    cout << "      ";
  }
  cout << data[root->index][root->dimension%data[root->index].size()] << endl;
  printTree(data, root->right);
  return;
}

vector<vector<double> > MultiThreadKD::randomData(int size_of_data, int dimension) {
  vector<vector<double> > v(size_of_data, vector<double>(dimension, 1));
  for (int i = 0; i < v.size(); ++i) {
    for (int j = 0; j < v[i].size(); ++j) {
      int sign = (rand()%2);
      if (sign) {
        v[i][j] = (double)((rand()%1000000)/(double)1000);
      } else {
        v[i][j] = -(double)((rand()%1000000)/(double)1000);
      }
    }
  }
  return v;
}

double MultiThreadKD::euclideanMetric(const vector<double>& first, const vector<double>& second) {
  assert(first.size() == second.size());
  double distance = 0;
  for (int i = 0; i < first.size(); ++i ) {
    distance += first[i]*first[i] + second[i]*second[i];
  }
  return sqrt(distance);
}

void MultiThreadKD::query(const vector<vector<double> >& data, Node* current, const vector<double>& p, vector<pair<double, Node*> >& knn) {
  if (current == NULL) return;
  double r = knn.back().first;
  double d = euclideanMetric(p, data[current->index]);
  if (d < r) {
    knn.back().first = d;
    knn.back().second = current;
    sort(knn.begin(), knn.end());
  }
  r = knn.back().first;
  double cuttingLine = data[current->index][current->dimension];
  bool first_check_left = false;
  if (p[current->dimension] <= cuttingLine) {
    // on the left
    query(data, current->left, p, knn);
    first_check_left = true;
  } else {
    query(data, current->right, p, knn);
  }

  double d_cross_dim = abs(cuttingLine-p[current->dimension]);
  if (first_check_left) {
    if (d_cross_dim < knn.back().first) {
      query(data, current->right, p, knn);
    }
  } else {
    if (d_cross_dim < knn.back().first) {
      query(data, current->left, p, knn);
    }
  }
  return;
}

void MultiThreadKD::checkMT(const vector<vector<double> >& single, const vector<vector<double> >& multi) {
  assert(single.size() == multi.size());
  for (int i = 0; i < single.size(); ++i) {
    assert(single[i].size() == multi[i].size());
    for (int j = 0; j < single[i].size(); ++j) {
      if (single[i][j] != multi[i][j]) {
        cout << single[i][j] << ", " << multi[i][j];
        cout << "Wrong answer!! " <<endl;
      }
    }
  }
}


int main(int argc, char const *argv[]) {
 srand(time(NULL));
 MultiThreadKD tree = MultiThreadKD(3);
 vector<vector<double> > data = tree.randomData(20, 5);
 tree.printData(data);
 Node* root = tree.buildTree(data);
 tree.printData(data);
 // prepareTree(data);
 tree.printTree(data, root);
 return 0;
}
