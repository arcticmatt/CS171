#include <Eigen/Dense>
#include <vector>

using Eigen::MatrixXd;
using namespace std;

MatrixXd compute_product(vector<MatrixXd> matrices);
MatrixXd get_matrix(char determ, float x, float y, float z, float angle = 0);
MatrixXd get_translation_matrix(float x, float y, float z);
MatrixXd get_scaling_matrix(float x, float y, float z);
MatrixXd get_rotation_matrix(float x, float y, float z, float angle);
