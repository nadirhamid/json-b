#include "jsonb.h"

int main(int argc, char* argv[])
{
  struct parser* parser;
  const char* str = "{\"name\": \"Falco\", \"age\": 4, \"bitable\": false}";
  jsonb_parse_from_str(&parser, str);
  jsonb_debugf("main ended parsing");
}
