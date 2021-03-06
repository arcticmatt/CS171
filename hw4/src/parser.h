#ifndef PARSER_H
#define PARSER_H

/*** For parsing graphics files. Contains a lot of copied code from Assignment 0 ***/
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <Eigen/Dense>
#include "framework.h"

using Eigen::MatrixXd;
using namespace std;

/*******************************************************************************
 * Defines methods needed for parsing files
 ******************************************************************************/

Scene *parse_scene(ifstream &infile);
vector<Object> object_list_converter(vector<Object *> pointer_list);
vector<Object *> parse_objects(ifstream &infile);
Object *parse_object(const char* filename);
MatrixXd compute_product(vector<MatrixXd> matrices);
MatrixXd get_matrix(char determ, float x, float y, float z, float angle = 0);
MatrixXd get_translation_matrix(float x, float y, float z);
MatrixXd get_scaling_matrix(float x, float y, float z);
MatrixXd get_rotation_matrix(float x, float y, float z, float angle);

#endif
