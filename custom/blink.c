/*
 * BUILD: gcc -o blink blink.c -lglfw -O3 ( -DSIMPLE )
 * A simpler alternative to lookaway for MacOS with
   the bare minimum functionality for X11.
 * NOTE: X11 does not offer keyboard passthrough
   so STOP typing during BLINKs
 * It blurs with a compositor else blacks out the screen.
 * It provides a BLINK for 5 sec every 15 minutes &
   a shorter WINK of 2 sec every 5 minutes.
 * Use "-DSIMPLE" to make both 3 sec.
 * It is paused by the existance of `/tmp/noblink` file.
 * Run as a deamon in '.xinitrc' or equivalent of you WM.
 * Written By: Boltzkreig
 */
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define GLFW_INCLUDE_NONE
#define WAIT (5*60)

#ifndef SIMPLE
#define BLINK 5
#define WINK 2
#define SKIP 3
#else
#define BLINK 3
#define WINK BLINK
#define SKIP 1
#endif

void hide(int count){
	if (!glfwInit()) return;

	const GLFWvidmode *VideoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	const int ScreenHeight       = VideoMode->height;
	const int ScreenWidth        = VideoMode->width;

	GLFWwindow* window           = glfwCreateWindow(ScreenWidth, ScreenHeight,
			"Curtain", glfwGetPrimaryMonitor(), NULL);
	glfwWindowHint(GLFW_DECORATED               , GLFW_FALSE ) ;
	glfwWindowHint(GLFW_FLOATING                , GLFW_TRUE  ) ;
	glfwWindowHint(GLFW_CENTER_CURSOR           , GLFW_FALSE ) ;
	glfwWindowHint(GLFW_FOCUSED                 , GLFW_FALSE);
	glfwWindowHint(GLFW_MOUSE_PASSTHROUGH       , GLFW_TRUE  ) ;
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER , GLFW_TRUE  ) ;
	glfwWindowHint(GLFW_AUTO_ICONIFY            , GLFW_FALSE ) ;
	glfwSetWindowOpacity(window                 , 0.2f       ) ;
	glfwSwapBuffers(window                                   ) ;

	if(count){ sleep(WINK); } else { sleep(BLINK); }
	glfwTerminate();
	return;
}

int main(){
	time_t now;
	int counter = 0;
	FILE *fd;
	while(1){
		now = time(NULL);
		sleep(WAIT - (now % WAIT));
		fd = fopen("/tmp/noblink","r");
		if(fd) { fclose(fd); } else {
			counter = (counter + 1) % SKIP;
			hide(counter);
		}
	}
	return 0;
}
/*
   static volatile bool force_quit = false;

   static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS || key == GLFW_KEY_Q && action == GLFW_PRESS)
   force_quit = true;

*/
