#pragma once
#include "EffectTarget.h"
#include "../../enums/EffectReason.h"
#include "../../ValueObjects/Money.h"

#include <stdexcept>

struct Effect {

	EffectTarget target;
	Money delta;
	EffectReason reason;

	Effect(EffectTarget target, const Money& delta, EffectReason reason)
	:target(target),delta(delta),reason(reason){
		if (target.targetId <= 0) {
			throw std::invalid_argument("Effect target id cannot be less or equal to zero");
		}
		if(delta == Money(0))
			throw std::invalid_argument("Effect delta cannot be zero");
	}

};


