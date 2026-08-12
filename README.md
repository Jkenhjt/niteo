# Niteo

A high-performance, **standalone** HTTP server library written in **Pure C**.

---

## Key Features

| Feature | Description |
| :------ | :---------- |
| **Language** | C (C11 compliant) |
| **Performance:** | 200k+ RPS / 20k connections |
| **Pure C Architecture:** | Written in standard C with **zero external dependencies**. |
| **Static Library Size:** | **~530kb** |
| **RAM Usage:** | For 20k connections, it used only **~100kb** of RAM **(only stack)**. |
| **Http parser:**| I created **libdexh1**, which uses a hash table to save HTTP fields (https://github.com/libdexh1) |

---

### Requirements

* A standard C compiler (`gcc` or `clang`)
* `make` utility

### Building

```bash
git clone https://github.com/user/niteo.git
cd niteo

make
sudo make install

# For specify folder for install
PREFIX=path make install
```

### Example

```c
#include <niteo/niteo.h>

void router_index(dexh1_http* req, dexh1_http* resp)
{
  NITEO_SET_RESPONSE_DATA(&resp->ht, "Hellow", 6);
}

int main(int argc, char** argv) {
  niteo_insert_router(&routers, router_index, "/", 1);
  niteo_server_launcher(0, 0); // If set 0, starts only one thread.

  return 0;
}
```
