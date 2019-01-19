#include <decs/decs.hpp>
#include <entt/entt.hpp>
#include <ecs/Registry.h>
#include <iostream>
#include <chrono>
#include <random>

#include "ExampleComponents.h"

namespace mud
{
	template <> struct TypedBuffer<Position> { static size_t index() { return 0; } };
	template <> struct TypedBuffer<Rotation> { static size_t index() { return 1; } };
	//template <> struct TypedBuffer<> { static size_t index() { return 2; } };
}

using namespace std;
BaseComponent::IDCounter BaseComponent::family_counter_ = 1;

struct timer final {
	timer() : start{ std::chrono::system_clock::now() } {}

	double elapsed() {
		auto now = std::chrono::system_clock::now();
		double time = std::chrono::nanoseconds(now - start).count() / 1000000.0;
		start = std::chrono::system_clock::now();
		return time;
	}

private:
	std::chrono::time_point<std::chrono::system_clock> start;
};

void Print_Comparaision(double entt, double decs, double ecs = 0.0, double raw = 0.0) {

	cout << "    entt took " << entt << " ms" << endl;
	cout << "    decs took " << decs << " ms" << endl;
	cout << "    mud ecs took " << ecs << " ms" << endl;
	cout << "    raw took " << raw << " ms" << endl;
	cout << "---------------------------------------------------" << endl;
}
void Compare_CreationDeletion()
{
	mud::ECS ecs;
	
	ECSWorld decs;
	decs.BlockStorage.reserve(100);
	entt::registry<> enttecs;

	Archetype EmptyArchetype;

	std::cout << "Constructing 1.000.000 entities-------------------------------" << std::endl;
	timer tim;
	//create entt entities
	for (std::uint64_t i = 0; i < 1000000L; i++) {
		enttecs.create();
	}
	double entt_creation = tim.elapsed();

	auto ets = decs.CreateEntityBatched(EmptyArchetype, 1000000L);

	double decs_creation = tim.elapsed();

	Print_Comparaision(entt_creation, decs_creation);


	std::cout << "Deleting 1.000.000 entities-------------------------------" << std::endl;

	tim.elapsed();

	enttecs.each([&](auto e) {
		enttecs.destroy(e);
	});
	entt_creation = tim.elapsed();

	for (auto e : ets)
	{
		decs.DeleteEntity(e);
	}
	decs_creation = tim.elapsed();
	Print_Comparaision(entt_creation, decs_creation);
}


struct Screamer {

	Screamer() {
	x = 0; 
	y = 1;
	z = 2;
	}
	~Screamer() {
	//	std::cout << "destroy";
	}
	
	float x, y, z;
};


void Compare_ComponentAdd()
{
	ECSWorld decs;
	decs.BlockStorage.reserve(10000);
	entt::registry<> enttecs;

	Archetype EmptyArchetype;

	std::cout << "Adding component to 1.000.000 entities-------------------------------" << std::endl;

	//create entt entities
	for (std::uint64_t i = 0; i < 1000000L; i++) {
		enttecs.create();
	}


	auto handles = decs.CreateEntityBatched(EmptyArchetype, 1000000L);



	timer tim;


	enttecs.each([&enttecs](auto entity) {
		enttecs.assign<Position>(entity);
	});
	double entt_creation = tim.elapsed();

	for (auto e : handles)
	{
		decs.AddComponent<Screamer>(e); 
		
	}
	double decs_creation = tim.elapsed();


	Print_Comparaision(entt_creation, decs_creation);
}

void Compare_ComponentRemove() {
	ECSWorld decs;
	decs.BlockStorage.reserve(100);
	entt::registry<> enttecs;


	Archetype PosRot;
	PosRot.AddComponent<Position>();
	PosRot.AddComponent<Rotation>();
	//PosRot.AddComponent<BigComponent>();

	std::cout << "Removing component to 1.000.000 entities-------------------------------" << std::endl;

	//create entt entities
	for (std::uint64_t i = 0; i < 1000000L; i++) {
		auto e = enttecs.create();
		enttecs.assign<Position>(e);
		enttecs.assign<Rotation>(e);
		//enttecs.assign<BigComponent>(e);
	}

	auto handles = decs.CreateEntityBatched(PosRot, 1000000L);

	timer tim;


	enttecs.each([&enttecs](auto entity) {
		enttecs.remove<Position>(entity);
	});
	double entt_creation = tim.elapsed();

	for (auto e : handles)
	{
		decs.RemoveComponent<Position>(e);
	}
	double decs_creation = tim.elapsed();


	Print_Comparaision(entt_creation, decs_creation);
}



void Compare_SimpleIteration()
{
	mud::ECS ecs;
	ECSWorld decs;
	decs.BlockStorage.reserve(100);
	entt::registry<> enttecs;


	Archetype PosRot;
	PosRot.AddComponent<Position>();
	PosRot.AddComponent<Rotation>();
	Archetype empty;
	Archetype PosOnly;
	PosOnly.AddComponent<Position>();
	//PosRot.AddComponent<BigComponent>();

	std::cout << "Iterating 1.000.000 entities 1 Component-------------------------------" << std::endl;

	//create entt entities
	for (std::uint64_t i = 0; i < 1000000L; i++) {
		auto e = enttecs.create();
		enttecs.assign<Position>(e);
		enttecs.assign<Rotation>(e);
		//enttecs.assign<BigComponent>(e);
	}

	auto handles = decs.CreateEntityBatched(PosRot, 1000000L);

	for(std::uint64_t i = 0; i < 1000000L; i++) {
		ecs.CreateEntity<Position, Rotation>();
	}

	std::vector<Position> positions;
	positions.resize(1000000L);

	std::mt19937 rng4(0);
	std::uniform_int_distribution<int> uniform_dist4(1, 10);

	std::mt19937 rng3(0);
	std::uniform_int_distribution<int> uniform_dist3(1, 10);

	std::mt19937 rng2(0);
	std::uniform_int_distribution<int> uniform_dist2(1, 10);

	std::mt19937 rng1(0);
	std::uniform_int_distribution<int> uniform_dist1(1, 10);

	int p = 0;

	enttecs.view<Position>().each([&](auto entity, auto& c) {
		p++;
		c.x = uniform_dist2(rng2);
		c.y = p;
	});
	p = 0;
	decs.IterateBlocks(PosOnly.componentlist, empty.componentlist, [&](ArchetypeBlock & block) {

		auto ap = block.GetComponentArray<Position>();

		for (int i = 0; i < block.last; i++)
		{
			p++;
			ap.Get(i).x = uniform_dist1(rng1);
			ap.Get(i).y = p;
		}
	});
	p = 0;
	ecs.Loop<Position>([&](Position& c)
	{
		p++;
		c.x = uniform_dist3(rng3);
		c.y = p;
	});
	p = 0;
	for(Position& c : positions)
	{
		p++;
		c.x = uniform_dist4(rng4);
		c.y = p;
	}

	timer tim;

	double entt_time = 0.0;
	double decs_time = 0.0;
	double ecs_time = 0.0;
	double raw_time = 0.0;
	int entt_compares = 0;
	int decs_compares = 0;
	int ecs_compares = 0;
	int raw_compares = 0;
	int entt_its = 0;
	int decs_its = 0;
	int ecs_its = 0;
	int raw_its = 0;

	for(size_t i = 0; i < 100; ++i)
	{
		enttecs.view<Position>().each([&](auto entity, const auto &c) {

			const int x = c.x;
			const int y = c.y;
			entt_compares += x % 20 + y;
			entt_its++;
		});
	}
	entt_time = tim.elapsed();

	for(size_t i = 0; i < 100; ++i)
	{
		decs.IterateBlocks(PosOnly.componentlist, empty.componentlist, [&](ArchetypeBlock & block) {

			auto ap = block.GetComponentArray<Position>();
			//auto ar = block.GetComponentArray<Rotation>();
			for(int i = 0; i < block.last; i++)


			{
				const int x = ap.Get(i).x;
				const int y = ap.Get(i).y;
				//cout << ":" << x << ";";
				decs_compares += x % 20 + y;
				decs_its++;
			}

		});
	}
	decs_time = tim.elapsed();

	for(size_t i = 0; i < 100; ++i)
	{
		ecs.Loop<Position>([&](const Position& c) {

			const int x = c.x;
			const int y = c.y;
			ecs_compares += x % 20 + y;
			ecs_its++;
		});
	}
	ecs_time = tim.elapsed();

	for(size_t i = 0; i < 100; ++i)
	{
		for(Position& c : positions)
		{
			const int x = c.x;
			const int y = c.y;
			raw_compares += x % 20 + y;
			raw_its++;
		}
	}
	raw_time = tim.elapsed();

	cout << "Numeric Results: " << entt_compares << ":" << decs_compares << ":" << ecs_compares << ":" << raw_compares << endl;
	cout << "Total Iterations: " << entt_its << ":" << decs_its << ":" << ecs_its << ":" << raw_its << endl;
	Print_Comparaision(entt_time, decs_time, ecs_time, raw_time);
}

void Compare_SimpleIteration_5Comps()
{
	ECSWorld decs;
	decs.BlockStorage.reserve(100);
	entt::registry<> enttecs;


	Archetype All;
	All.AddComponent<Position>();
	All.AddComponent<Rotation>();
	All.AddComponent<C1>();
	All.AddComponent<C2>();
	All.AddComponent<C2>();
	Archetype empty;
	Archetype PosOnly;
	PosOnly.AddComponent<Position>();
	//PosRot.AddComponent<BigComponent>();

	std::cout << "Iterating 1.000.000 entities 5 Component-------------------------------" << std::endl;

	//create entt entities
	for (std::uint64_t i = 0; i < 1000000L; i++) {
		auto e = enttecs.create();
		enttecs.assign<Position>(e);
		enttecs.assign<Rotation>(e);
		enttecs.assign<C1>(e);
		enttecs.assign<C2>(e);
		enttecs.assign<C3>(e);
		//enttecs.assign<BigComponent>(e);
	}

	auto handles = decs.CreateEntityBatched(All, 1000000L);
	auto view = enttecs.persistent_view<Position, Rotation, C1, C2, C3>();
	//view.initialize();

	timer tim;

	int compares = 0;
	view.each([&](auto entity, auto &c, auto &c1, auto &c2, auto &c3, auto &c4) {
		c.x = c1.y * c2.z;
		compares+= c.x;
	});
	double entt_creation = tim.elapsed();

	int compares2 = 0;
	decs.IterateBlocks(All.componentlist, empty.componentlist, [&](ArchetypeBlock & block) {
		auto ap = block.GetComponentArray<Position>();
		auto ar = block.GetComponentArray<Rotation>();
		auto c1 = block.GetComponentArray<C1>();

		for (int i = 0; i < block.last; i++)
		{
			ap.Get(i).x = ar.Get(i).y * c1.Get(i).z;
			compares2+= ap.Get(i).x;
		}
	});
	double decs_creation = tim.elapsed();

	cout << "Total Iterations: " << compares << ":" << compares2 << endl;
	Print_Comparaision(entt_creation, decs_creation);
}
float DummyFunction(C1 & ca, C2&cb, C3 & cc)
{
	ca.x = cb.x + cc.y;
	cb.y = ca.z / cc.x;
	cc.x = ca.x;
	cc.z = cb.x * cb.y;
	return cc.z;
}

void AddRandomComponentsToEntity(entt::registry<>::entity_type & enttEntity ,EntityHandle & decsEntity, ECSWorld& decs,entt::registry<>&enttecs,  std::mt19937 & rng,std::uniform_int_distribution<int>&uniform_dist) {

	if (uniform_dist(rng) < 4)
	{
		enttecs.assign<Position>(enttEntity);
		decs.AddComponent<Position>(decsEntity);
	}
	if (uniform_dist(rng) < 4)
	{
		enttecs.assign<Rotation>(enttEntity);
		decs.AddComponent<Rotation>(decsEntity);
	}
	if (uniform_dist(rng) < 4)
	{
		enttecs.assign<C1>(enttEntity);
		decs.AddComponent<C1>(decsEntity);
	}
	if (uniform_dist(rng) < 4)
	{
		enttecs.assign<C2>(enttEntity);
		decs.AddComponent<C2>(decsEntity);
	}
	if (uniform_dist(rng) < 4)
	{
		enttecs.assign<C3>(enttEntity);
		decs.AddComponent<C3>(decsEntity);
	}
	if (uniform_dist(rng) < 4)
	{
		enttecs.assign<comp<1>>(enttEntity);
		decs.AddComponent<comp<1>>(decsEntity);
	}
	if (uniform_dist(rng) < 4)
	{
		enttecs.assign<comp<2>>(enttEntity);
		decs.AddComponent<comp<2>>(decsEntity);
	}
	if (uniform_dist(rng) < 4)
	{
		enttecs.assign<comp<3>>(enttEntity);
		decs.AddComponent<comp<3>>(decsEntity);
	}
	if (uniform_dist(rng) < 4)
	{
		enttecs.assign<comp<4>>(enttEntity);
		decs.AddComponent<comp<4>>(decsEntity);
	}
	if (uniform_dist(rng) < 4)
	{
		enttecs.assign<comp<5>>(enttEntity);
		decs.AddComponent<comp<5>>(decsEntity);
	}
}

#define MATCH_5 1

void Compare_Iteration_Pathological(uint64_t numEntities, bool bShuffle = false )
{
	ECSWorld decs;
	decs.BlockStorage.reserve(100000);
	entt::registry<> enttecs;

	int MatchNumber = 3;

	Archetype All;
	All.AddComponent<Position>();
	All.AddComponent<Rotation>();
	All.AddComponent<C1>();
	All.AddComponent<C2>();
	All.AddComponent<C2>();
	Archetype empty;
	Archetype PosOnly;
	PosOnly.AddComponent<Position>();
	//PosRot.AddComponent<BigComponent>();
	Archetype Cs;
	Cs.AddComponent<C1>();
	Cs.AddComponent<C2>();
	Cs.AddComponent<C3>();

#if MATCH_5
	Cs.AddComponent<comp<1>>();
	Cs.AddComponent<comp<2>>();
	MatchNumber = 5;
#endif 

	std::cout << "Iterating "<< (bShuffle ? " shuffled " : " ordered ")  << "entities: pathological random case 10 Component find "<<MatchNumber <<": Num Entities : " << numEntities <<"  -------------------------------" << std::endl;

	std::mt19937 rng(0);
	std::uniform_int_distribution<int> uniform_dist(1, 10);

	std::vector<entt::registry<>::entity_type > enttEntities;
	std::vector<EntityHandle> decsEntities;

	enttEntities.reserve(numEntities);
	decsEntities.reserve(numEntities);
	//create entt entities
	for (std::uint64_t i = 0; i < numEntities; i++) {
		auto e1 = enttecs.create();
		auto e2 = decs.CreateEntity(empty);
		enttEntities.push_back(e1);
		decsEntities.push_back(e2);


		AddRandomComponentsToEntity(e1,e2,decs,enttecs,rng,uniform_dist);
	}
	//delete half the entities randomly

	if (bShuffle)
	{
		for (int i = 0; i < numEntities; i++)
		{
			if (uniform_dist(rng) < 5)
			{
				enttecs.destroy(enttEntities[i]);
				decs.destroy(decsEntities[i]);

				auto e1 = enttecs.create();
				auto e2 = decs.CreateEntity(empty);
				AddRandomComponentsToEntity(e1, e2, decs, enttecs, rng, uniform_dist);
			}
		}
	}




	//auto handles = decs.CreateEntityBatched(All, 1000000L);
	auto view = enttecs.persistent_view<C1, C2, C3
#if MATCH_5
		,comp<1>,comp<2>
#endif
	
	>();
	//view.initialize();

	timer tim;

	long long compares = 0;
	

	int nIterationsEnTT = 0;
	view.each([&](auto entity, auto &c2, auto &c3, auto &c4
#if MATCH_5
		,auto &c5, auto &c6
#endif
		)
														  
	{// {
		compares+=DummyFunction(c2,c3,c4);
		nIterationsEnTT++;
	});
	double entt_creation = tim.elapsed();
	long long  compares2 = 0;
	int nIterationsdecs = 0;
	for (int i = 0; i < 1; i++)
	{
	decs.IterateBlocks(Cs.componentlist, [&](ArchetypeBlock & block) {
		auto ap = block.GetComponentArray<C1>();
		auto ar = block.GetComponentArray<C2>();
		auto c1 = block.GetComponentArray<C3>();

		for (int i = 0; i < block.last; i++)
		{
			nIterationsdecs++;
			compares2+=DummyFunction(ap.Get(i),ar.Get(i),c1.Get(i));
			
			
		}
	});
	}
	double decs_creation = tim.elapsed();

	int compares3 = 0;
	decs.IterateBlocks(Cs.componentlist, [&](ArchetypeBlock & block) {
		auto ap = block.GetComponentArray<C1>();
		auto ar = block.GetComponentArray<C2>();
		auto c1 = block.GetComponentArray<C3>();

		for (int i = 0; i < block.last; i++)
		{
			//compares3++;
			compares3+=DummyFunction(ap.Get(i),ar.Get(i),c1.Get(i));
		}
	}, true);
	tim.elapsed();
	compares3 = 0;
	decs.IterateBlocks(Cs.componentlist, [&](ArchetypeBlock & block) {

		auto ap = block.GetComponentArray<C1>();
		auto ar = block.GetComponentArray<C2>();
		auto c1 = block.GetComponentArray<C3>();

		for (int i = 0; i < block.last; i++)
		{
			
			compares3+=DummyFunction(ap.Get(i),ar.Get(i),c1.Get(i));
		}
	}, true);

	double decs_parallel = tim.elapsed();

	cout << "Total Iterations: " << nIterationsEnTT << ":" << nIterationsdecs << endl;
	cout << "Dummy Numbers: " << compares << ":" << compares2 << endl;
	cout << "    decs Parallel: " << decs_parallel << "ms" << endl;
	Print_Comparaision(entt_creation, decs_creation);
}

int main()
{
	
		//Compare_Creation();

		//cout << "===========Comparing decs against entt ================" << endl << endl;
	for (int i = 0; i < 1; i++)
	{
		Compare_CreationDeletion();
		Compare_ComponentAdd();
		Compare_ComponentRemove();
	}
		Compare_SimpleIteration();
		Compare_SimpleIteration_5Comps();
		for (int i = 0; i < 3; i++)
		{
			Compare_Iteration_Pathological(1000000, false);
		}
		for (int i = 0; i < 3; i++)
		{
			Compare_Iteration_Pathological(1000000, true);
		}
	for (int i = 5; i < 20; i++)
	{
		Compare_Iteration_Pathological(i * 20000);
	}

	

	char a;
	cin >> a;
}