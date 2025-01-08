#ifndef ERROR_H
#define ERROR_H

template<typename T>
class Error {
  private:
    T info;
  public:
    Error(T info) : info(info) {}
    T get_info() const { return info; }
};

#endif
