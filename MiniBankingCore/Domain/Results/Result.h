#pragma once
#include "iostream"

template<typename T, typename E>
class Result {
    //tag dispatch
    struct OkTag {};
    struct ErrTag {};


private:
    T _value;
    E _error;
    bool _isSuccess;

private:
    Result(OkTag,T value) :_value(value),_isSuccess(true) {}
    Result(ErrTag,E error) :_error(error),_isSuccess(false) {}
public:
    static Result Ok(T value) { return Result(OkTag{},value); }
    static Result Err(E error) { return Result(ErrTag{},error); }

    bool IsOk() const { return _isSuccess; }
    T Value() const { return _value; }
    E Error() const { return _error; }
};