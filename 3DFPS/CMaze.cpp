#include "stdafx.h"
#include "CMaze.h"
#include <random>
#include <vector>

CMaze::CMaze()
	: m_pMazeData		(nullptr)
	, m_RegionWidth		(0)
	, m_RegionHeight	(0)
	, m_Stride			(0)
	, m_HasExit			(false)
	, m_ExitCell		({ 0, 0 })
	, m_ExitWorldPos	(0.f, 0.f, 0.f)
{
}

CMaze::CMaze(int* pMaze, int stride, int regionWidth, int regionHeight, int startX, int startY)
	: m_pMazeData(pMaze)
	, m_RegionWidth(regionWidth)
	, m_RegionHeight(regionHeight)
	, m_Stride(stride)
	, m_HasExit(false)
	, m_ExitCell({ 0, 0 })
	, m_ExitWorldPos(0.f, 0.f, 0.f)
{
}

CMaze::~CMaze()
{

	if (m_pMazeData != nullptr)
	{
		delete m_pMazeData;
		m_pMazeData = nullptr;
	}

}

D3DXVECTOR3 CMaze::CellToWorld(int cellIndex, float y, float cellSize) const
{
	const int total = m_RegionWidth * m_RegionHeight;
	if (cellIndex < 0) cellIndex = 0;
	if (cellIndex >= total) cellIndex = total - 1;

	const int row = cellIndex / m_RegionWidth;
	const int col = cellIndex % m_RegionWidth;

	return CellToWorldRC(row, col, y, cellSize);
}

D3DXVECTOR3 CMaze::CellToWorldRC(int row, int col, float y, float cellSize) const
{
	if (row < 0) row = 0;
	if (row >= m_RegionHeight) row = m_RegionHeight - 1;
	if (col < 0) col = 0;
	if (col >= m_RegionWidth) col = m_RegionWidth - 1;

	const float x = (col - m_RegionWidth ) * cellSize;
	const float z = -(row - m_RegionHeight ) * cellSize ;
	return D3DXVECTOR3(x, y, z);
}

D3DXVECTOR3 CMaze::CellToWorld(int cellIndex, int height, int width, float y, float cellSize)
{
	return D3DXVECTOR3();
}

D3DXVECTOR3 CMaze::CellToWorldRC(int row, int col, int height, int width, float y, float cellSize)
{
	if (row < 0) row = 0;
	if (row >= height) row = height - 1;
	if (col < 0) col = 0;
	if (col >= width) col = width - 1;

	const float x = (col - width) * cellSize;
	const float z = -(row - height) * cellSize;
	return D3DXVECTOR3(x, y, z);
}

std::vector<Pair> CMaze::GeneratePath(int x, int y)
{
	std::vector<Pair> fullPath;

	if (!IsInBounds(x, y, m_RegionWidth, m_RegionHeight) || m_pMazeData == nullptr || m_Stride <= 0)
		return fullPath;

	const int w = m_RegionWidth;
	const int h = m_RegionHeight;

	//using smart pointer
	std::unique_ptr<bool[]> visited(new bool[w * h]());
	auto toIndex = [w](int cx, int cy) { return cy * w + cx; };
	std::vector<Pair> st;

	visited[toIndex(x, y)] = true;
	st.emplace_back(x, y);
	fullPath.emplace_back(x, y);

	while (!st.empty())
	{
		const int cx = st.back().x;
		const int cy = st.back().y;

		bool moved = false;
		Direction dirs[4] = { North, South, East, West };
		ShuffleDirections(dirs, 4);

		for (int i = 0; i < 4; ++i)
		{
			const Direction dir = dirs[i];
			if (!IsValidPathMove(cx, cy, dir, visited.get()))
				continue;

			const Pair d = GetMovementFromDirection(dir);
			const int nx = cx + d.x;
			const int ny = cy + d.y;

			visited[toIndex(nx, ny)] = true;
			st.emplace_back(nx, ny);
			fullPath.emplace_back(nx, ny);
			moved = true;
			break; // dive deeper (DFS)
		}

		if (!moved)
		{
			// Dead end -> backtrack
			st.pop_back();
			if (!st.empty())
			{
				// Add the previous cell to the path to trace back
				fullPath.push_back(st.back());
			}
		}
	}

	return fullPath;
}

void CMaze::GenerateMaze(int startX, int startY)
{
	GenerateMaze(m_pMazeData, m_Stride, m_RegionWidth, m_RegionHeight, startX, startY);
	AddRandomLoops(m_pMazeData, m_Stride, m_RegionWidth, m_RegionHeight, 0.10f);
	GenerateRandomExit(startX, startY);
}


// 迷路を生成
void CMaze::GenerateMaze(int* out, int stride, int regionWidth, int regionHeight, int startX, int startY)
{

	for (int y = 0; y < regionHeight; ++y)
		for (int x = 0; x < regionWidth; ++x)
			out[y * stride + x] = 0;

	//範囲内に開始位置をクランプ
	ClampStart(startX, startY, regionWidth, regionHeight);
	// 通路を掘る
	CarvePassages(startX, startY, out, stride, regionWidth, regionHeight);

}

void CMaze::CarveEntrances(int* maze, int stride, int regionWidth, int regionHeight)
{
		
	int entranceX = 0;
	int entranceY = 0;

	int exitX = regionWidth - 1;
	int exitY = regionHeight - 1;

	maze[entranceY * stride + entranceX] |= West;
	maze[exitY * stride + exitX]		 |= East;

}

void CMaze::BuildMazeBorders(int* maze, int stride, int regionWidth, int regionHeight)
{
	for (int y = 0; y < regionHeight; ++y)
	{
		for (int x = 0; x < regionWidth; ++x)
		{
			const int idx = y * stride + x;
			if (y == 0)
			{
				maze[idx] &= ~North;
			}
			if (y == regionHeight - 1)
			{
				maze[idx] &= ~South;
			}
			if (x == 0)
			{
				maze[idx] &= ~West;
			}
			if (x == regionWidth - 1)
			{
				maze[idx] &= ~East;
			}
		}
	}
}

// 通路を掘る再帰関数
void CMaze::CarvePassages(int cx, int cy, int* maze, int stride, int regionWidth, int regionHeight)
{
	// ランダムな方向の配列を作成
	Direction directions[4] = { North, South, East, West };
	ShuffleDirections(directions, 4);

	// 現在のセルのインデックスを計算
	// 説明: strideは1行あたりのセル数を示す
	const int curIdx = cy * stride + cx;

	for (int i = 0; i < 4; ++i)
	{
		const Direction dir = directions[i];
		const Pair d = GetMovementFromDirection(dir);
		const int nx = cx + d.x;
		const int ny = cy + d.y;

		if (!IsInBounds(nx, ny, regionWidth, regionHeight))
			continue;

		const int nextIdx = ny * stride + nx;
		if (maze[nextIdx] != 0)
			continue;

		// 現在のセルと隣接セルの間の壁を取り除く
		maze[curIdx]	|= dir;
		maze[nextIdx]	|= GetOppositeDirection(dir);

		CarvePassages(nx, ny, maze, stride, regionWidth, regionHeight);
	}

}

// 指定された方向の反対方向を取得
CMaze::Direction CMaze::GetOppositeDirection(Direction dir)
{
	switch (dir)
	{
		case North:
			return South;
		case South:
			return North;
		case East:
			return West;
		case West:
			return East;
	}
}
// 指定された方向に対応する移動量を取得
Pair CMaze::GetMovementFromDirection(Direction dir)
{
	switch (dir)
	{
	case North:
		return { 0, -1 };
	case South:
		return { 0,  1 };
	case East:
		return { 1, 0};
	case West:
		return { -1, 0};
	}
}
// 指定された座標が迷路の範囲内にあるか確認
bool CMaze::IsInBounds(int x, int y, int width, int height)
{
	return (x >= 0 && x < (width) && y >= 0 && y < (height));
}

bool CMaze::IsValidPathMove(int x, int y, Direction dir, const bool* visited)
{
	if (m_pMazeData == nullptr || visited == nullptr)
		return false;

	// 現在セルの通路
	const int curIdx = y * m_Stride + x;
	if ((m_pMazeData[curIdx] & dir) == 0)
		return false;

	// 移動先
	const Pair d = GetMovementFromDirection(dir);
	const int nx = x + d.x;
	const int ny = y + d.y;

	if (!IsInBounds(nx, ny, m_RegionWidth, m_RegionHeight))
		return false;

	// 移動先が未訪問か
	const int nvi = ny * m_RegionWidth + nx;
	if (visited[nvi])
		return false;

	return true;
}


// 配列内の方向をランダムにシャッフル
void CMaze::ShuffleDirections(Direction* directions, int size)
{
	if (directions == nullptr || size <= 1)
		return;
	
	static thread_local std::mt19937 rng{ std::random_device{}() };
	for (int i = size - 1; i > 0; --i)
	{
		std::uniform_int_distribution<int> dist(0, i);
		const int j = dist(rng);
		const Direction tmp = directions[i];
		directions[i] = directions[j];
		directions[j] = tmp;
	}
}
// 開始位置を迷路の範囲内にクランプ
void CMaze::ClampStart(int& startX, int& startY, int regionWidth, int regionHeight)
{
	if (startX < 0) startX = 0;
	if (startY < 0) startY = 0;
	if (startX >= regionWidth)  startX = regionWidth - 1;
	if (startY >= regionHeight) startY = regionHeight - 1;
}

void CMaze::AddRandomLoops(int* maze, int stride, int regionWidth, int regionHeight, float probability)
{
	if (maze == nullptr || stride <= 0 || regionWidth <= 0 || regionHeight <= 0)
		return;
	if (probability <= 0.0f)
		return;

	static thread_local std::mt19937 rng{ std::random_device{}() };
	std::uniform_real_distribution<float> chance(0.0f, 1.0f);

	// Iterate all cells and consider opening a random neighboring wall
	for (int y = 0; y < regionHeight; ++y)
	{
		for (int x = 0; x < regionWidth; ++x)
		{
			// Candidate directions; avoid double-processing by only considering East/South
			const CMaze::Direction candidates[2] = { East, South };
			for (int i = 0; i < 2; ++i)
			{
				const CMaze::Direction dir = candidates[i];
				const Pair d = GetMovementFromDirection(dir);
				const int nx = x + d.x;
				const int ny = y + d.y;

				if (!IsInBounds(nx, ny, regionWidth, regionHeight))
					continue;

				const int curIdx = y * stride + x;
				const int nextIdx = ny * stride + nx;

				// If a wall exists between current and neighbor, we can potentially open it
				const bool closedForward = (maze[curIdx] & dir) == 0;
				const bool closedBack = (maze[nextIdx] & GetOppositeDirection(dir)) == 0;

				if (closedForward && closedBack)
				{
					// Randomly open to create a loop
					if (chance(rng) < probability)
					{
						maze[curIdx] |= dir;
						maze[nextIdx] |= GetOppositeDirection(dir);
					}
				}
			}
		}
	}
}

void CMaze::GenerateRandomExit(int startX, int startY)
{
	// Prefer dead-ends (cells with exactly one open passage)
	std::vector<Pair> candidates;
	candidates.reserve(m_RegionHeight * m_RegionWidth / 4);

	for (int r = 0; r < m_RegionHeight; ++r)
	{
		for (int c = 0; c < m_RegionWidth; ++c)
		{
			const int cellBits = m_pMazeData[r * m_Stride + c];
			const int openCount = __popcnt(cellBits); // number of open directions

			if (openCount == 1)
			{
				if (!(r == startY && c == startX))
				{
					// Store as {row, col}
					candidates.push_back({ r, c });
				}
			}
		}
	}

	// Fallback to border cells if no dead-ends found
	if (candidates.empty())
	{
		for (int r = 0; r < m_RegionHeight; ++r)
		{
			for (int c = 0; c < m_RegionWidth; ++c)
			{
				const bool isBorder = (r == 0) || (c == 0) || (r == (m_RegionHeight - 1)) || (c == (m_RegionWidth - 1));
				if (isBorder && !(r == startY && c == startX))
				{
					candidates.push_back({ r, c });
				}
			}
		}
	}

	// Absolute fallback: bottom-right
	if (candidates.empty())
	{
		m_ExitCell = { m_RegionHeight - 1, m_RegionWidth - 1 };
	}
	else
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
		m_ExitCell = candidates[dist(gen)];
	}

	m_HasExit = true;
}

D3DXVECTOR3 CMaze::GetExitWorldPosition(float y, float cellSize) const
{
	if (!m_HasExit)
	{
		// Safe default if not generated yet
		return CellToWorldRC(m_RegionHeight - 1, m_RegionWidth - 1, y, cellSize);
	}
	return CellToWorldRC(m_ExitCell.x, m_ExitCell.y, y, cellSize);
}