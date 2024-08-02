#include <map>
#include <functional>

class ResponseHandlerRegistry
{
private:
  std::map<uint8_t, std::function<void(uint8_t *, uint8_t)>> responseHandlers;

public:
  void registerHandler(uint8_t commandCode, std::function<void(uint8_t *, uint8_t)> handler)
  {
    responseHandlers[commandCode] = handler;
  }

  void handleResponse(uint8_t *data, uint8_t length)
  {
    uint8_t commandCode = data[0];
    if (responseHandlers.find(commandCode) != responseHandlers.end())
    {
      responseHandlers[commandCode](data, length);
    }
  }
};