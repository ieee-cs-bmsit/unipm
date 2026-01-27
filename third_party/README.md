# Third-Party Dependencies

## nlohmann/json

unipm uses the single-header [nlohmann/json](https://github.com/nlohmann/json) library for JSON parsing.

### Installation

Download the latest release:
```bash
# Linux/macOS
wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp -O third_party/json.hpp

# Windows PowerShell
Invoke-WebRequest -Uri "https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp" -OutFile "third_party\json.hpp"
```

Or use curl:
```bash
curl -L https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp -o third_party/json.hpp
```

### License

MIT License - Copyright (c) 2013-2022 Niels Lohmann

The library is included as a single header file and does not require separate compilation.
