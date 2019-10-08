#pragma once

#include<easyAR/vector.hpp>
#include<easyAR/camera.h>
#include<easyAR/framestreamer.hpp>
#include<easyAR/imagetarget.hpp>
#include<easyAR/imagetracker.hpp>
#include<easyar/renderer.hpp>
#include<easyar/player.hpp>

#include<GL/glut.h>
#include<gl/glut.h>
#include<cmath>
#include<string>
#include<algorithm>
#include<memory>
#include<vector>
#include<unordered_map>
#include<cstdio>

#include<opencv2/opencv.hpp>
#include<vector>

using namespace cv;
using namespace std;




bool initialize();
void finalize();
bool start();
bool stop();
void nextFrame();

std::shared_ptr<easyar::Frame> m_nextFrame(GLuint &texID, float matrix[16]);
void getProjection(float projectionMat[16]);
vector<vector<Point>> getContours(Mat img);
Mat HSVBin(Mat img);
Mat Frame2Mat(std::shared_ptr<easyar::Frame> frame);
