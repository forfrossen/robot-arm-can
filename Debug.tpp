template <typename T>
void Debug::add(T value, int format)
{
  if (format == -1)
  {
    messageBuffer.push_back(String(value));
  }
  else
  {
    messageBuffer.push_back(String(value, format));
  }
}

template <typename T>
void Debug::print(T value, int format)
{
  if (format == -1)
  {
    add(value);
  }
  else
  {
    add(String(value, format));
  }
  println();
}
