#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>
#include <cmath>
#include <vector>
#include "Tree.h"

class Framework
{
public:
	// Contructor which initialize the parameters.
	Framework(int height_, int width_): height(height_), width(width_)
	{
		TTF_Init();
		SDL_Init(SDL_INIT_VIDEO);       // Initializing SDL as Video
		SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);      // setting draw color
		SDL_RenderClear(renderer);      // Clear the newly created window
		SDL_RenderPresent(renderer);    // Reflects the changes done in the
		//  window.
	}

	// Destructor
	~Framework()
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void draw_line(int x1, int y1, int x2, int y2, int r, int g, int b)
	{
		SDL_SetRenderDrawColor(renderer, r, g, b, 255);
		SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

		SDL_RenderPresent(renderer);
	}

	void draw_rectangle(int x, int w, int y, int h, int r, int g, int b)
	{
		SDL_SetRenderDrawColor(renderer, r, g, b, 255);

		SDL_Rect rectangle;
		rectangle.x = x;
		rectangle.w = w;
		rectangle.y = y;
		rectangle.h = h;

		SDL_RenderFillRect(renderer, &rectangle);

		SDL_RenderPresent(renderer);
	}

	void draw_fat_line(double x1, double y1, double x2, double y2, int width, int r, int g, int b)
	{
		x1 -= width / 2;
		y1 -= width / 2;
		x2 -= width / 2;
		y2 -= width / 2;

		double xDif = std::abs(x1 - x2);
		double yDif = std::abs(y1 - y2);

		double xChange;
		double yChange;

		if(xDif > yDif)
		{
			xChange = x1 < x2 ? 1 : -1;
			yChange = (yDif / xDif) * (y1 < y2 ? 1 : -1);
		} else
		{
			xChange = (xDif / yDif) * (x1 < x2 ? 1 : -1);
			yChange = y1 < y2 ? 1 : -1;
		}
		
		for(double x = x1, y = y1; ((x1 < x2) ? (x < x2) : (x > x2)) && ((y1 < y2) ? (y < y2) : (y > y2)); x += xChange, y += yChange)
		{
			draw_rectangle(x, width, y, width, r, g, b);
		}
	}

	void draw_text(int fontSize, std::string text, int x, int w, int y, int h, int  r, int g, int b)
	{
		TTF_Font *Font = TTF_OpenFont("Arial.ttf", fontSize);
		SDL_Color textColor = {r, g, b, 0};
		SDL_Surface *textSurface = TTF_RenderText_Solid(Font, text.c_str(), textColor);
		SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_FreeSurface(textSurface);
		SDL_Rect renderQuad = {x, y, w, h};
		SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);

		// Show the change on the screen
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(textTexture);
	}

	void draw_circle(int center_x, int center_y, int radius_, int r, int g, int b)
	{
		// Setting the color to be RED with 100% opaque (0% trasparent).
		SDL_SetRenderDrawColor(renderer, r, g, b, 255);

		for(int y = 0; y <= radius_; y++)
		{
			int x = std::round(std::sqrt(std::pow(radius_, 2) - std::pow(y, 2)));
			SDL_RenderDrawLine(renderer, center_x - x, center_y + y, center_x + x, center_y + y);
			SDL_RenderDrawLine(renderer, center_x - x, center_y - y, center_x + x, center_y - y);
		}

		// Show the change on the screen
		SDL_RenderPresent(renderer);
	}

	std::string GetLeftTesting(Node *node)
	{
		if(node->Child1 == nullptr)
		{
			return std::to_string(node->ID) + ',';
		}
		return GetLeftTesting(node->Child1) + GetLeftTesting(node->Child2);
	}

	std::string GetNodeTesting(Node *node)
	{
		if(node->Child1 == nullptr)
		{
			return '{' + std::to_string(node->ID) + '}';
		}
		std::string lefts = GetLeftTesting(node->Child1);
		return '{' + lefts.substr(0, lefts.size() - 1) + '}';
	}

	void draw_node(Node *node, int radius_, int r, int g, int b, int hDif, int wDif)
	{
		draw_rectangle(xByPos(radius_, wDif, node) - radius_, radius_ * 2, yByLevel(radius_, hDif, node) - radius_, radius_ * 2, r, g, b);
		if(node->Child1 != nullptr)
		{
			draw_line(xByPos(radius_, wDif, node), yByLevel(radius_, hDif, node) + radius_, xByPos(radius_, wDif, node->Child1), yByLevel(radius_, hDif, node->Child1) - radius_, 255, 0, 0);
		}
		if(node->Child2 != nullptr)
		{
			draw_line(xByPos(radius_, wDif, node), yByLevel(radius_, hDif, node) + radius_, xByPos(radius_, wDif, node->Child2), yByLevel(radius_, hDif, node->Child2) - radius_, 255, 0, 0);
		}
		draw_text(radius_ / 2, std::to_string(node->Weight).substr(0, std::to_string(node->Weight).find('.') + 4), xByPos(radius_, wDif, node) - radius_, radius_ * 2, yByLevel(radius_, hDif, node) - radius_, radius_ * 2, 0, 0, 0);
		draw_text(radius_ / 2, GetNodeTesting(node), xByPos(radius_, wDif, node) - radius_, radius_ * 2, yByLevel(radius_, hDif, node) + radius_, radius_ * 2, 0, 0, 0);
	}
	int yByLevel(int radius, int hDif, Node *node)
	{
		return radius * 2 * node->Y + hDif * (1 + node->Y);
	}
	int xByPos(int radius, int wDif, Node *node)
	{
		return radius * 2 * node->X + wDif * (1 + node->X) + radius;
	}

	void drawAllNodes(Node *head, int radius, int hDif, int wDif)
	{
		int r = head->Child1 != nullptr ? 0 : 255;
		int g = head->Child1 != nullptr ? 255 : 0;
		int b = 0;

		draw_node(head, radius, r, g, b, hDif, wDif);
		if(head->Child1 != nullptr)
		{
			drawAllNodes(head->Child1, radius, hDif, wDif);
		}
		if(head->Child2 != nullptr)
		{
			drawAllNodes(head->Child2, radius, hDif, wDif);
		}
	}

	void draw_tree(const std::vector<double> &Weights, const std::vector<int> &IDs, bool valley = false)
	{
		draw_rectangle(0, width, 0, height, 255, 255, 255);
		Tree tree(Weights, IDs, valley);
		int ry = height / (4 * (tree.MaxLevel + 1) + 3);
		int rx = 2 * width / (5 * Weights.size() + 1);

		int radius = ry;
		if(radius > rx)
			radius = rx;

		int hDif = (height - radius * 2 * (tree.MaxLevel + 1)) / (tree.MaxLevel + 2);
		int wDif = (width - radius * 2 * Weights.size()) / (Weights.size() + 1);

		drawAllNodes(tree.Head, radius, hDif, wDif);
	}
	void screenshot(std::string imagePath)
	{
		SDL_Surface *surf = SDL_GetWindowSurface(window);
		SDL_RenderReadPixels(renderer, nullptr, 0, surf->pixels, surf->pitch);
		IMG_SavePNG(surf, imagePath.c_str());
	}
	void clear()
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

private:
	int height;     // Height of the window
	int width;      // Width of the window
	SDL_Renderer *renderer = NULL;      // Pointer for the renderer
	SDL_Window *window = NULL;      // Pointer for the window
};