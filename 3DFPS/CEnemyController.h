#pragma once
class CEnemyController
{

public:

	CEnemyController();
	~CEnemyController();
	void Update(float deltaTime);
	void TakeDamage(int damage);
	bool IsAlive() const;
};

