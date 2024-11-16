# ArpScan

### 请给我 Star 🌟，非常感谢！这对我很重要！

### Please give me Star 🌟, thank you very much! It is very important to me!

### 1. 介绍

https://github.com/HackerCalico/ArpScan

ARP 主机存活探测器，对 C 段进行乱序扫描 (OPSEC)，支持随时修改线程数、延迟、暂停、停止。

### 2. 使用场景

在内网扫描阶段，先探测主机存活再针对存活主机深入扫描是一个 OPSEC 的策略。

使用 ARP 来进行主机探测是一个合适的选择，ARP 在内网通信中起着非常重要的作用，它不像 ICMP 是防火墙的默认拦截项。

ARP 请求是不能通过 Socks5 代理的，意味着你必须上传扫描器才能进行扫描。但是功能完善的扫描器体积大且特征明显，不是很 OPSEC。此时你就需要这样一个易于改造与 BOF 化的超轻量级扫描器来完成主机探测，再利用其他扫描器 + 代理来完成深入扫描。