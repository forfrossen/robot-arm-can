#ifndef SIMPLEVECTOR_H
#define SIMPLEVECTOR_H

template <typename T>
class SimpleVector
{
private:
  T *data;
  size_t capacity;
  size_t size;

  void resize()
  {
    capacity *= 2;
    T *newData = new T[capacity];
    for (size_t i = 0; i < size; ++i)
    {
      newData[i] = data[i];
    }
    delete[] data;
    data = newData;
  }

public:
  SimpleVector() : capacity(10), size(0)
  {
    data = new T[capacity];
  }

  ~SimpleVector()
  {
    delete[] data;
  }

  void push_back(const T &value)
  {
    if (size == capacity)
    {
      resize();
    }
    data[size++] = value;
  }

  size_t getSize() const
  {
    return size;
  }

  T &operator[](size_t index)
  {
    return data[index];
  }

  const T &operator[](size_t index) const
  {
    return data[index];
  }
};

#endif