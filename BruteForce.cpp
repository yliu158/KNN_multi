#include "KD_Tree.cpp"
#include <limits.h>

int size_of_data = 20;
int dimension = 2;
int n = 5;

void knn(const vector<vector<double> >& data, const vector<double>& p, vector<pair<double, double> >& res) {
  for (int i = 0; i < size_of_data; ++i) {
    double d = euclideanMetric(p, data[i]);
    if (res.back().first > d) {
      res.back().first = d;
      res.back().second = (double)i;
      sort(res.begin(), res.end());
    }
  }
  return;
}

void randomDataTest() {
  srand(time(NULL));
  vector<vector<double> > data = randomData(size_of_data, dimension);
  printData(data);
  Node* root = buildTree(data);
  printTree(data, root);
  vector<double> p;
  for (int j = 0; j < dimension; ++j) {
    p.push_back((double)((rand()%1000000)/(double)1000));
  }

  vector< pair<double, double> > res_bf(n,pair<double, double>(numeric_limits<double>::max() , 0));
  vector< pair<double, Node*> > res_kd(n, pair<double, Node*>(numeric_limits<double>::max() , NULL));
  knn(data, p ,res_bf);
  query(data, root, p, res_kd);
  for (int i = 0; i < res_bf.size(); ++i) {
    cout << "BF" <<res_bf[i].first  << "  [" << res_bf[i].second << "]" << endl;
    cout << "KD" <<res_kd[i].first  << "  [" << res_kd[i].second->index << "]" << endl;
    if (res_bf[i].first != res_kd[i].first || res_bf[i].second != res_kd[i].second->index) {
      cout << "Wrong Answer!!! \n";
    }
  }
}

void fileDataTest() {
  srand(time(NULL));
  vector<vector<double> > *data = readTrainingFile("data_1000_2.dat");
  Node* root = buildTree(*data);
  printData(*data);
  printTree(*data, root);
  vector<vector<double> > * queries = readQueryFile("query_10_2_5.dat", n);
  vector<double> p = queries->at(0);

  vector< pair<double, double> > res_bf(n,pair<double, double>(numeric_limits<double>::max() , 0));
  vector< pair<double, Node*> > res_kd(n, pair<double, Node*>(numeric_limits<double>::max() , NULL));
  knn(*data, p ,res_bf);
  query(*data, root, p, res_kd);
  for (int i = 0; i < res_bf.size(); ++i) {
    cout << "BF" <<res_bf[i].first  << "  [" << res_bf[i].second << "]" << endl;
    cout << "KD" <<res_kd[i].first  << "  [" << res_kd[i].second->index << "]" << endl;
    if (res_bf[i].first != res_kd[i].first || res_bf[i].second != res_kd[i].second->index) {
      cout << "Wrong Answer!!! \n";
    }
  }
}


int main(int argc, char const *argv[]) {
  // fileDataTest();
  randomDataTest();
  return 0;
}
