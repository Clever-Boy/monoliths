#pragma once
#include "stdafx.h"
#include "common.h"
#include "Monolith.h"
#include <random>

class MonolithGenerator
{
private:
	typedef std::normal_distribution<float> NormalDistribution;
	typedef std::uniform_real_distribution<float> UniformDistribution;
	typedef std::exponential_distribution<float> ExponentialDistribution;

	std::default_random_engine _engine;
	NormalDistribution _sideDistribution;
	NormalDistribution _heightDistribution;
	ExponentialDistribution _scaleDistribution;

	UniformDistribution _positionDistribution;
	UniformDistribution _angleDistribution;

	ExponentialDistribution _slantDistribution;

	float _slantScaledown;
	float _lowering;
	float _characterRadius;
	float _characterHeight;

	float getPositive(NormalDistribution dist)
	{
		float val = dist(_engine);
		if (val < 0) val = dist.mean()*0.5;
		return val;
	}

public:

	MonolithGenerator(float mapSize,
		float sideMean, float sideSigma, 
		float heightMean, float heightSigma, 
		float scaleLambda, float slantLambda, float slantScaledown, float lowering,
		float characterRadius = DEFAULT_CHARACTER_RADIUS*2, float characterHeight = MAX_CHARACTER_HEIGHT)
		: 
#ifdef _DEBUG
	_engine(42),
#else
	_engine(std::random_device()()),
#endif
		  
		  
		  _sideDistribution(sideMean, sideSigma),
		  _heightDistribution(heightMean, heightSigma),
		  _scaleDistribution(scaleLambda),
		  _positionDistribution(-mapSize*0.5f, mapSize*0.5f),
		  _angleDistribution(0, Math::PI*2),
		  _slantDistribution(slantLambda),
		  _slantScaledown(slantScaledown),
		  _lowering(lowering),
		  _characterRadius(characterRadius),
		  _characterHeight(characterHeight)
	{
	}

	Monolith* createMonolith()
	{
		float a = getPositive(_sideDistribution);
		float b = getPositive(_heightDistribution);
		float c = getPositive(_sideDistribution);
		float s = 1+_scaleDistribution(_engine);

		float x = _positionDistribution(_engine);
		float z = _positionDistribution(_engine);
		float ang = _angleDistribution(_engine);

		float slant = _slantDistribution(_engine)*_slantScaledown;
		//Vector3 up(slant, 1,0);
		//up.normalise();

		return new Monolith(a*s, b*s+_lowering, c*s, x, -_lowering, z, ang, slant, _characterRadius, _characterHeight);
	}
};
