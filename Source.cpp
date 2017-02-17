#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>

using namespace std;

const int xres = GetSystemMetrics(0);
const int yres = GetSystemMetrics(1);
const int pixelPerCheck = 10;
int colR = 0, colG = 0, colB = 0, firstBLuePosx, firstBLuePosy, middleBoxPosX, middleBoxPosy, blockWidth, blockHight, blocksInRow, blockInColumn, spaceX = 0, spaceY = 0;
bool win = false, refresh = false, fckingAnimation = true ,itisStuck = false;
HDC hdc, hdcTemp;
BYTE* bite;
string field;
vector<int> blockCoordinatesX;
vector<int> blockCoordinatesY;
vector<int> blockStatus;

void searchingStartingPoint()	//looking for specific blue color. Blue color == first square of minesweeper
{
	int reducetXres = xres / pixelPerCheck;
	int reducetYres = (yres - 50) / pixelPerCheck;

	for (int x = 0; x < reducetXres; x++)
	{	
		for (int y = 0; y < reducetYres; y++)
		{			
			colB = bite[(y * pixelPerCheck * xres * 3) + (x * pixelPerCheck * 3) + 0];
			colG = bite[(y * pixelPerCheck * xres * 3) + (x * pixelPerCheck * 3) + 1];
			colR = bite[(y * pixelPerCheck * xres * 3) + (x * pixelPerCheck * 3) + 2];
			if (colB == 255 && colR < 140 && colR >100 && colG < 240 && colG > 190)
			{
				firstBLuePosx = x * pixelPerCheck;
				firstBLuePosy = y * pixelPerCheck;
				goto end_this_magical_loops;	//I love spaghetti 
			}
		}
	}
	end_this_magical_loops: {}
	SetCursorPos(firstBLuePosx, firstBLuePosy);
	cout << "\nSearching for starting position 1/3 done" << endl;
}

void rightClick(int x, int y) {
	SetCursorPos(x, y);
	mouse_event(MOUSEEVENTF_RIGHTDOWN, x, y, 0, 0);
	mouse_event(MOUSEEVENTF_RIGHTUP, x, y, 0, 0);
}

void leftClick(int x, int y) {
		SetCursorPos(x, y);
		mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
}

void coutField() {
	int blockStatusChart = 0;
	for (int i = 0; i < blocksInRow; i++) {
		for (int a = 0; a < blockInColumn; a++) {
			field += to_string(blockStatus[blockStatusChart + blockInColumn*a]) + " " ;
		}
		field += "\n";
		blockStatusChart++;
	}
	cout << field << endl;
	field = "";
}

void countingBlockScaleX(int x,int y) {
	int midcounting = x;
	int midcounting2 = x ;

	blockWidth = 0;

	while (colB > 190) {
		midcounting--;
		colB = bite[(y * xres * 3) + (midcounting * 3) + 3];
		blockWidth++;
	}

	colB = 255;
	while (colB > 190) {
		midcounting2++;
		colB = bite[(y * xres * 3) + (midcounting2 * 3) + 3];
		blockWidth++;
	}

}

void countingMiddlePointsX(int x,int y) {
	int clonOfFirstBluePosx = x;
	colB = 255;
	while (colB > 80) {
		clonOfFirstBluePosx--;
		colB = bite[(y * xres * 3) + (clonOfFirstBluePosx * 3) + 3];
	}
	middleBoxPosX = clonOfFirstBluePosx + (blockWidth / 2)+1;
}

void countingBlockScaleY(int x, int y) {
	int midcounting = y;
	int midcounting2 = y;

	blockHight = 0;

	colB = 255;
	while (colB >  190) {
		midcounting--;
		colB = bite[(midcounting * xres * 3) + (x * 3) + 3];
		blockHight++;
	}

	colB = 255;
	while (colB >  190) {
		midcounting2++;
		colB = bite[(midcounting2 * xres * 3) + (x * 3) + 3];
		blockHight++;
	}
}

void countingMiddlePointsY(int x, int y) {
	int clonOfFirstBluePosy = y;
	colB = 255;
	while (colB > 80) {
		clonOfFirstBluePosy--;
		colB = bite[(clonOfFirstBluePosy * xres * 3) + (x * 3) + 3];
	}
	middleBoxPosy = clonOfFirstBluePosy + (blockHight/ 2);
}

int chceckColor(int r,int b,int g) {
	 if (r < 80 && g < 220 && g>120 && b < 250 && b>190)
		return 1;
	else if (r > 50 && r < 170 && g>130 && g < 180 && b < 80 && b>15)
		return 2;
	else if (r > 130 && r < 240 && g < 120 && b < 130 && b>70 && b!=g)
		return 3;
	else if (r < 100 && g < 120 && g>70 && b < 220 && b>170)
		return 4;
	else if (g> 20 && g == b && r > 100 && r < 230)
		return 5;
	else if (r < 50 && g>95 && g < 160 && b < 80 && b>40)
		return 6;
	else if (g < 70 && r == b)
		return 7;
	else if (r > 170 && r < 255 && g>70 && g < 170 && b>20 && b < 110)
		return 8;

	else
	{
		return 0;
	}
}

void blockCoordinatesCount() {
	countingBlockScaleX(firstBLuePosx, firstBLuePosy);
	countingMiddlePointsX(firstBLuePosx, firstBLuePosy);

	blockCoordinatesX.push_back(1);
	blockCoordinatesX[0] = middleBoxPosX;

	countingBlockScaleY(middleBoxPosX, firstBLuePosy);
	countingMiddlePointsY(middleBoxPosX, firstBLuePosy);

	blockCoordinatesY.push_back(1);
	blockCoordinatesY[0] = middleBoxPosy;


	colB = 255;
	while (true) {  //count middle points X
		int pointX = middleBoxPosX + blockWidth;
		colB = bite[(middleBoxPosy * xres * 3) + (pointX * 3) + 0];
		if (colB > 190) {
			countingBlockScaleX(pointX, middleBoxPosy);
			countingMiddlePointsX(pointX, middleBoxPosy);

			blockCoordinatesX.push_back(1);
			blockCoordinatesX[blockCoordinatesX.size()-1] = middleBoxPosX;			
		}
		else
		{
			break;
		}
	}
	cout << "Counting center of blocks 2/3 done" << endl;

	colB = 255;
	while (true) {  //count middle points Y
		int pointY = middleBoxPosy + blockHight;
		colB = bite[(pointY * xres * 3) + (middleBoxPosX * 3) + 0];
		if (colB > 190) {
			countingBlockScaleY(middleBoxPosX, pointY);
			countingMiddlePointsY(middleBoxPosX, pointY);

			blockCoordinatesY.push_back(1);
			blockCoordinatesY[blockCoordinatesY.size() - 1] = middleBoxPosy;		
		}
		else
		{
			break;
		}
	}
}

void rescaningLocation(bool lastCheck)
{
	HDC hdc, hdcTemp;
	BYTE* bite;
	int red, green, blue;

	hdc = GetDC(NULL);

	hdcTemp = CreateCompatibleDC(hdc);
	BITMAPINFO bitmap;
	bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
	bitmap.bmiHeader.biWidth = xres;
	bitmap.bmiHeader.biHeight = -yres;
	bitmap.bmiHeader.biPlanes = 1;
	bitmap.bmiHeader.biBitCount = 24;
	bitmap.bmiHeader.biCompression = BI_RGB;
	bitmap.bmiHeader.biSizeImage = 0;
	bitmap.bmiHeader.biClrUsed = 0;
	bitmap.bmiHeader.biClrImportant = 0;

	HBITMAP bmp = CreateDIBSection(hdcTemp, &bitmap, DIB_RGB_COLORS, (void**)&bite, NULL, NULL);
	if (bmp == NULL)
		cout << "Something went Wrong";

	SelectObject(hdcTemp, bmp);
	BitBlt(hdcTemp, 0, 0, xres, yres, hdc, 0, 0, SRCCOPY);

	if (lastCheck == false) {
		blockStatus.clear();
		blockStatus.shrink_to_fit();

		for (int x = 0; x < blocksInRow; x++)
		{
			for (int y = 0; y < blockInColumn; y++) {
				blockStatus.push_back(0);
				bool symbolFind = false;

				if (symbolFind == false) {
					red = bite[(blockCoordinatesY[y] * xres * 3) + (blockCoordinatesX[x] * 3) + 2];
					blue = bite[(blockCoordinatesY[y] * xres * 3) + (blockCoordinatesX[x] * 3) + 0];
					green = bite[(blockCoordinatesY[y] * xres * 3) + (blockCoordinatesX[x] * 3) + 1];

					int chceckColorReturnValue = chceckColor(red, blue, green);
					if (chceckColorReturnValue != 0) {
						blockStatus[blockStatus.size() - 1] = chceckColor(red, blue, green);
						symbolFind = true;
					}
				}

				if (symbolFind == false) {
					red = bite[(blockCoordinatesY[y] * xres * 3) + ((blockCoordinatesX[x] + 1) * 3) + 2];
					blue = bite[(blockCoordinatesY[y] * xres * 3) + ((blockCoordinatesX[x] + 1) * 3) + 0];
					green = bite[(blockCoordinatesY[y] * xres * 3) + ((blockCoordinatesX[x] + 1) * 3) + 1];

					int chceckColorReturnValue = chceckColor(red, blue, green);
					if (chceckColorReturnValue != 0) {
						blockStatus[blockStatus.size() - 1] = chceckColor(red, blue, green);
						symbolFind = true;
					}
				}

				if (symbolFind == false) {
					red = bite[(blockCoordinatesY[y] * xres * 3) + ((blockCoordinatesX[x] + 2) * 3) + 2];
					blue = bite[(blockCoordinatesY[y] * xres * 3) + ((blockCoordinatesX[x] + 2) * 3) + 0];
					green = bite[(blockCoordinatesY[y] * xres * 3) + ((blockCoordinatesX[x] + 2) * 3) + 1];

					int chceckColorReturnValue = chceckColor(red, blue, green);
					if (chceckColorReturnValue != 0) {
						blockStatus[blockStatus.size() - 1] = chceckColor(red, blue, green);
						symbolFind = true;
					}
				}


				if (symbolFind == false) {
					red = bite[(blockCoordinatesY[y] * xres * 3) + ((blockCoordinatesX[x] + 3) * 3) + 2];
					blue = bite[(blockCoordinatesY[y] * xres * 3) + ((blockCoordinatesX[x] + 3) * 3) + 0];
					green = bite[(blockCoordinatesY[y] * xres * 3) + ((blockCoordinatesX[x] + 3) * 3) + 1];

					int chceckColorReturnValue = chceckColor(red, blue, green);
					if (chceckColorReturnValue != 0) {
						blockStatus[blockStatus.size() - 1] = chceckColor(red, blue, green);
						symbolFind = true;
					}
				}


				if (symbolFind == false) {
					red = bite[((blockCoordinatesY[y] - 1) * xres * 3) + ((blockCoordinatesX[x]) * 3) + 2];
					blue = bite[((blockCoordinatesY[y] - 1) * xres * 3) + ((blockCoordinatesX[x]) * 3) + 0];
					green = bite[((blockCoordinatesY[y] - 1) * xres * 3) + ((blockCoordinatesX[x]) * 3) + 1];

					int chceckColorReturnValue = chceckColor(red, blue, green);
					if (chceckColorReturnValue != 0) {
						blockStatus[blockStatus.size() - 1] = chceckColor(red, blue, green);
						symbolFind = true;
					}
				}

				if (symbolFind == false) {
					red = bite[((blockCoordinatesY[y] + 1) * xres * 3) + ((blockCoordinatesX[x]) * 3) + 2];
					blue = bite[((blockCoordinatesY[y] + 1) * xres * 3) + ((blockCoordinatesX[x]) * 3) + 0];
					green = bite[((blockCoordinatesY[y] + 1) * xres * 3) + ((blockCoordinatesX[x]) * 3) + 1];

					int chceckColorReturnValue = chceckColor(red, blue, green);
					if (chceckColorReturnValue != 0) {
						blockStatus[blockStatus.size() - 1] = chceckColor(red, blue, green);
						symbolFind = true;
					}
				}


				if (blockStatus[blockStatus.size() - 1] == 0) {
					if (red < 200)
						blockStatus[blockStatus.size() - 1] = 9;
				}
			}
		}

		DeleteObject(bmp);
		DeleteDC(hdcTemp);
	}
	else {
		red = bite[(blockCoordinatesY[0] * xres * 3) + ((blockCoordinatesX[0]) * 3) + 2];
		blue = bite[(blockCoordinatesY[0] * xres * 3) + ((blockCoordinatesX[0]) * 3) + 0];
		green = bite[(blockCoordinatesY[0] * xres * 3) + ((blockCoordinatesX[0]) * 3) + 1];

		if (red == green &&green == blue && red < 240 && red>5) {			
			win = true;
		}

		DeleteObject(bmp);
		DeleteDC(hdcTemp);
	}
}

int main()
{
	while (true) {
		system("Pause");
		cout << "Working...";

		hdc = GetDC(NULL);
		hdcTemp = CreateCompatibleDC(hdc);
		BITMAPINFO bitmap;
		bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
		bitmap.bmiHeader.biWidth = xres;
		bitmap.bmiHeader.biHeight = -yres;
		bitmap.bmiHeader.biPlanes = 1;
		bitmap.bmiHeader.biBitCount = 24;
		bitmap.bmiHeader.biCompression = BI_RGB;
		bitmap.bmiHeader.biSizeImage = 0;
		bitmap.bmiHeader.biClrUsed = 0;
		bitmap.bmiHeader.biClrImportant = 0;

		HBITMAP bmp = CreateDIBSection(hdcTemp, &bitmap, DIB_RGB_COLORS, (void**)&bite, NULL, NULL);
		if (bmp == NULL)
			cout << "Something went Wrong";

		SelectObject(hdcTemp, bmp);
		BitBlt(hdcTemp, 0, 0, xres, yres, hdc, 0, 0, SRCCOPY);

		searchingStartingPoint();
		blockCoordinatesCount();

		blocksInRow = blockCoordinatesX.size();
		blockInColumn = blockCoordinatesY.size();

		DeleteObject(bmp);
		DeleteDC(hdcTemp);

		//cout << blockWidth << "\t" << blockHight << endl;
		//cout << blockInColumn << "\t" << blocksInRow << endl;
		//system("pause");
		//SetCursorPos(blockCoordinatesX[0], blockCoordinatesY[0]);
		//system("pause");

		int ***block;										//create 3D dynamical array	
		block = new int **[blocksInRow];
		for (int i = 0; i < blocksInRow; i++) {
			block[i] = new int *[blockInColumn];
			for (int a = 0; a < blockInColumn; a++) {
				block[i][a] = new int[4];					//0 == x-coordinates | 1 == y-coordinates |	2 == block mine status 11 == defused mine  1 == 1 2 == 2 3 == 3 4 == 4 5 == 5 6 == 6 7 == 7 8 == 8  9 =blue block 0 nothing here | 3 if 0 this block can be refresh if 1 do not refresh

				block[i][a][0] = blockCoordinatesX[i];
				block[i][a][1] = blockCoordinatesY[a];
				block[i][a][3] = 0;
			}
		}


		cout << "Counting center of blocks 3/3 done \nEverything is ready! \nShall we?" << endl;
		leftClick(block[0][0][0], block[0][0][1]);
		Sleep(300);

		rescaningLocation(0);

		int blockStatusChart = 0;
		for (int i = 0; i < blocksInRow; i++) {			//aktualizovanie oznacenia
			for (int a = 0; a < blockInColumn; a++) {
				block[i][a][2] = blockStatus[blockStatusChart];
				blockStatusChart++;
			}
		}

		srand(time(NULL));

		while (win == false) {
			itisStuck = true;

			for (int i = 0; i < blocksInRow; i++) {
				for (int a = 0; a < blockInColumn; a++) {
					if (block[i][a][2] == 1 || block[i][a][2] == 2 || block[i][a][2] == 3 || block[i][a][2] == 4 || block[i][a][2] == 5 || block[i][a][2] == 6 || block[i][a][2] == 7 || block[i][a][2] == 8 && block[i][a][3] == 0 && win == false)
					{

						if (refresh == true) {

							if (fckingAnimation == true) {
								Sleep(120);
							}

							rescaningLocation(0);

							int blockStatusChart = 0;
							for (int q = 0; q < blocksInRow; q++) {			//aktualizovanie oznacenia
								for (int w = 0; w < blockInColumn; w++) {
									if (block[q][w][3] == 0)
										block[q][w][2] = blockStatus[blockStatusChart];
									blockStatusChart++;
								}
							}

							//thread t1(coutField);
							//t1.detach();

							refresh = false;
						}

						int defused = 0, unknow = 0;
						for (int x = -1; x <= 1; x++) {				//search unknown block and defused mines
							for (int y = -1; y < 2; y++) {
								if (i + x >= 0 && i + x < blocksInRow && a + y >= 0 && a + y < blockInColumn)
								{
									if (block[i + x][a + y][2] == 11)
										defused++;
									else if (block[i + x][a + y][2] == 9)
										unknow++;
								}
							}
						}

						if (block[i][a][2] == defused + unknow) {			//click on blue block which is probably mine
							for (int x = -1; x <= 1; x++) {
								for (int y = -1; y <= 1; y++) {
									if (i + x >= 0 && i + x < blocksInRow && a + y >= 0 && a + y < blockInColumn)
									{
										if (block[i + x][a + y][2] == 9) {
											rightClick(block[i + x][a + y][0], block[i + x][a + y][1]);
											refresh = true;
											itisStuck = false;
											block[i + x][a + y][2] = 11;
											block[i + x][a + y][3] = 1;
											block[i][a][3] = 1;
										}
									}
								}
							}
						}

						else if (block[i][a][2] == defused) {				//click on blue block which is not mine
							for (int x = -1; x <= 1; x++) {
								for (int y = -1; y <= 1; y++) {
									if (i + x >= 0 && i + x < blocksInRow && a + y >= 0 && a + y < blockInColumn) {
										if (block[i + x][a + y][2] == 9) {

											rescaningLocation(1);       //try if we win
											if (win == false) {
												leftClick(block[i + x][a + y][0], block[i + x][a + y][1]);
												refresh = true;
												itisStuck = false;
												fckingAnimation = false;					//FALSE = fast but risky TRUE = safe but slower
												block[i][a][3] = 1;
											}
										}
									}
								}
							}
						}

					}
				}
			}

			if (itisStuck == true) {								//random click if there is not another possible way
				int unknowBlock = 0;
				for (int i = 0; i < blocksInRow; i++) {
					for (int a = 0; a < blockInColumn; a++) {
						if (block[i][a][2] == 9) {
							unknowBlock++;
						}

					}
				}

				int randClickOn = rand() % unknowBlock + 1;
				for (int i = 0; i < blocksInRow; i++) {
					for (int a = 0; a < blockInColumn; a++) {
						if (block[i][a][2] == 9) {
							randClickOn--;
							if (randClickOn == 0) {
								rescaningLocation(1);
								if (win == false) {
									leftClick(block[i][a][0], block[i][a][1]);
									Sleep(50);
									refresh = true;
								}
							}
						}
					}
				}

			}
		}
		win = false;

		blockCoordinatesX.clear();
		blockCoordinatesX.shrink_to_fit();

		blockCoordinatesY.clear();
		blockCoordinatesY.shrink_to_fit();

		blockStatus.clear();
		blockStatus.shrink_to_fit();
	}
	return 0;
}