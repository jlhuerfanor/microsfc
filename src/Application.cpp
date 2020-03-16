/*
Copyright 2020 Jerson Leonardo Huerfano Romero

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "Application.h"

namespace sfc {

Application::Application(const component_context_t & container_context)
	: StatefulObject()
{
	this->container_context = container_context;
	this->component_delta = 0;
	this->evaluate = false;
}

Application::Application() : StatefulObject() {
	this->container_context = { {NULL, 0}, {NULL, 0}, { NULL, 0 } };
	this->component_delta = 0;
	this->evaluate = false;
}

Application::~Application() { }

void Application::stateReported(const stateful_state_t &state) {
	
}

void Application::evaluateStates(const sfc::ulong_t &delta) {
	this->component_delta = delta;

	if(PTR_ACTIVATING(this->getState())) {
		for(size_t i = 0; i < this->getStepCount(); i++) {
			if(this->isEntryPoint(i)) {
				this->toggleStepState(i, true);
			}
		}
	} else if(PTR_DEACTIVATING(this->getState())) {
		for(size_t i = 0; i < this->getStepCount(); i++) {
			this->toggleStepState(i, false);
		}
	} else if(PTR_ACTIVATED(this->getState())) {
		this->evaluateTransitions();
		this->evaluateActions();
	}
}

void Application::evaluateTransitions() {
	for(size_t i = 0; i < this->container_context.transitions.size; i++) {
		Transition * transition = (this->container_context.transitions.ptr) + i;
		transition->onActivationChanged(this);
	}
}
void Application::evaluateActions() {
	for(size_t i = 0; i < this->container_context.actions.size; i++) {
		Action * action = *((this->container_context.actions.ptr) + i);
		action->evaluate(this);
	}
}

void Application::performComponentTick(const sfc::ulong_t &delta) {
	for(size_t i = 0; i < this->container_context.steps.size; i++) {
		Step * step = (this->container_context.steps.ptr) + i; 
		step->onTick(delta + component_delta);
	}
	for(size_t i = 0; i < this->container_context.actions.size; i++) {
		Action * action = *ARRAY_GET(this->container_context.actions, i); 
		action->onTick(delta + component_delta);
		continue;
	}
}

void Application::onTick(const sfc::ulong_t &delta) {
	StatefulObject::onTick(delta);
	
	evaluate = !evaluate;

	if(evaluate) {
		evaluateStates(delta);
	} else {
		performComponentTick(delta);
	}
}

const sfc::stateful_state_t& Application::getStepState(const int &id) {
	return *(ARRAY_GET(this->container_context.steps, id)->getState());
}

size_t Application::getStepCount() {
	return this->container_context.steps.size;
}

bool Application::isEntryPoint(const int &id) {
	Step * step = this->container_context.steps.ptr + id;
	return step->isEntryPoint();
}

void Application::toggleStepState(const int &id, const bool &active) {
	if(active) {
		(this->container_context.steps.ptr + id)->activate();
	} else {
		(this->container_context.steps.ptr + id)->shutdown();
	}
}

component_context_t * const Application::getContext() {
	return &(this->container_context);
}

} /* namespace sfc */
