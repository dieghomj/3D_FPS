#pragma once
#include <vector>

class CMaze
{

public:

	enum Direction
	{
		North = 1,
		South = 2,
		East = 4,
		West = 8
	};




public:

	CMaze();
	CMaze(int* pMaze, int stride, int regionWidth, int regionHeight, int startX = 0, int startY = 0);
	~CMaze();


	void SetRegionSize(int regionWidth, int regionHeight) { m_RegionWidth = regionHeight; m_RegionHeight = regionHeight; };
	int* GetMazeData() const { return m_pMazeData; }
	int GetRegionWidth() const { return m_RegionWidth; }
	int GetRegionHeight() const { return m_RegionHeight; }
	int GetStride() const { return m_Stride; }
	Pair GetExitCell() const { return m_ExitCell; };
	D3DXVECTOR3 GetExitWorldPosition(float y, float cellSize) const;

	D3DXVECTOR3 CellToWorld(int cellIndex, float y = 0.f, float cellSize = 4.0f) const;
	D3DXVECTOR3 CellToWorldRC(int row, int col, float y = 0.f, float cellSize = 4.0f) const;
	static D3DXVECTOR3 CellToWorld(int cellIndex, int height, int width, float y = 0.f, float cellSize = 4.0f);
	static D3DXVECTOR3 CellToWorldRC(int row, int col, int height = 1.f, int width = 1.f, float y = 0.f, float cellSize = 4.0f);

	std::vector<Pair> GeneratePath(int x, int y);
	void GenerateMaze(int startX, int startY);

	static void GenerateMaze(int* out, int stride, int regionWidth, int regionHeight, int startX = 0, int startY = 0);

private:

	static void CarvePassages(int cx, int cy, int* maze, int stride, int regionWidth, int regionHeight);
	static void CarveEntrances(int* maze, int stride, int regionWidth, int regionHeight);
	static void BuildMazeBorders(int* maze, int stride, int regionWidth, int regionHeight);
	
	static Direction GetOppositeDirection(Direction dir);
	static Pair GetMovementFromDirection(Direction dir);
	static bool IsInBounds(int x, int y, int width, int height);
	static void ClampStart(int& startX, int& startY, int regionWidth, int regionHeight);
	static void ShuffleDirections(Direction* directions, int size);
	
	bool IsValidPathMove(int x, int y, Direction dir, const bool* visited);
	void AddRandomLoops(int* maze, int stride, int regionWidth, int regionHeight, float probability);
	void GenerateRandomExit(int startX, int startY);

private: 

	int*	m_pMazeData;
	int		m_RegionWidth;
	int		m_RegionHeight;
	int		m_Stride;

	Pair         m_ExitCell = { 0, 0 };
	D3DXVECTOR3  m_ExitWorldPos = D3DXVECTOR3(0.f, 0.f, 0.f);
	bool m_HasExit;
};

