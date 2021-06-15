#pragma once

#include <mutex>
#include <thread>


template <typename T>
class Singleton{

	Singleton(const Singleton&) = delete;
	Singleton &operator=(const Singleton &) = delete;

public:
	Singleton() {}
	~Singleton(){
		if(s_instance){
			delete s_instance;
			s_instance=nullptr;
		}
	}

	static T *pointer(){
		if(s_instance){
			return s_instance;
		}
		std::call_once(s_once,[&](){
			s_instance = new T();  
		});
		return s_instance;
	}

	template <class... P>
	static T &Instance(P... args){
		if(s_instance){
			return *s_instance;
		}
		std::call_once(s_once,[&](){
			s_instance = new T(args...);
		});
		return *s_instance;
	}

	static T &Instance(){
		if(s_instance){
			return *s_instance;
		}
		std::call_once(s_once,[&](){
			s_instance = new T();
		});
		return *s_instance;
	}

private:
	static std::once_flag s_once;
	static T *s_instance;
};

template <typename T>
std::once_flag Singleton<T>::s_once;
template <typename T>
T *Singleton<T>::s_instance=nullptr;
