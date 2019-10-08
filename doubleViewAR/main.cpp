// doubleViewAR.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


int globalInt = 0;
#include"m_openGL.h"
#include "m_easyAR.h"

#include <easyAR/engine.hpp>

#include <string>
#include <cstdio>
#include <iostream>
#include <thread>

#include<gl/freeglut.h>


int main(int argc, char **argv)
{
	auto isQuited = false;

	auto result = easyar::Engine::initialize("kS8CbS2ShQv4Y59zr50fKyVPT2HysV0EAHHDcSlN5DhbULSDaAR65orOqSTq9Q80odMxVnqBGeWJpECAJe4fJ0GqEHRieCYaWl0oGECoAYY0xUifzW5HTTpQWbRCp597rzBSEfXICvyBqTxdXliq03Sb9Cck3EmgQpVFzS1FAKmRsPJ4iiIPa4fyZ2UNnSqHsDqdW3fC");
	if (!result) {
		std::printf("initialization failed\n");
		return 1;
	}
	if (!initialize()) {
		std::printf("initialization failed\n");
		return 1;
	}
	start();
	initOpenGL(argc, argv);
	glutMainLoop();
	stop();
	finalize();
	
	return 0;
}

