# mosaic_gnss_driver

## Build workspace

- Clone this repository **[ branch : libdriver-devel ]**

- Build

```bash
bash ws_build.sh
```

### Other commands

- Cleanup binaries and documentation

```bash
bash ws_build.sh clean
```

- Generate documentation

```bash
bash ws_build.sh gendoc
```

## Notes

- Wireshark installation for capturing traffic from module

  - Install wireshark

  ```bash
  sudo add-apt-repository ppa:wireshark-dev/stable
  sudo apt update
  sudo apt -y install wireshark
  ```

  > When asked whether to allow non-superusers to capture packets, select **<Yes\>** and finish installation.

  - Configure wireshark

  ```bash
  sudo usermod -aG wireshark $USER
  ```

  - Change `dumpcap` binary file permissions

  ```bash
  sudo chgrp wireshark /usr/bin/dumpcap
  sudo chmod 750 /usr/bin/dumpcap
  sudo setcap cap_net_raw,cap_net_admin=eip /usr/bin/dumpcap
  ```

  - Verify configuration

  ```bash
  sudo getcap /usr/bin/dumpcap
  ```

  Expected output : `/usr/bin/dumpcap = cap_net_admin,cap_net_raw+eip`
  <br>

- Install C++ libraries for traffic playback

  ```bash
  sudo apt install libpcap-dev
  ```
