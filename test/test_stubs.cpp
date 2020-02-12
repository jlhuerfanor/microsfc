/*
 * test_stubs.cpp
 *
 *  Created on: 23/11/2019
 *      Author: leonardo
 */

#include "../test/test_stubs.h"

const stateful_state_t deactivatedState = { 1, false, false, false };
const stateful_state_t activatingState = { 0, true, true, true };
const stateful_state_t activeState = { 1, true, true, false };
const stateful_state_t deactivatingState = { 0, false, false, true };
int input_ids[] = { 0, 1, 2 };
int output_ids[] = { 3, 4, 5 };

Timer _5_tick_timer = Timer(5);

bool active_after_5_ticks(const predicate_state_t &state) {
	if (ACTIVATING(state.step_state)) {
		_5_tick_timer.reset();
		_5_tick_timer.enable();
	}
	_5_tick_timer.onTick(1);
	return _5_tick_timer.getState()->interrupted;
}

bool predicate_always_true(const stateful_state_t &state) {
	(void) state;
	return true;
}
bool predicate_always_false(const stateful_state_t &state) {
	(void) state;
	return false;
}
