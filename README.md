# Simple-UDP-Rat
UDP 공격 기능을 가진 단순한 Rat. Salsa20으로 통신 암호화.

## Client
- Rat의 Bot 소스코드
- Tcp를 통해 통신하며 모든 데이터는 수정된 Salsa20 알고리즘으로 암호화됨
- Google coding style을 따르려 노력
- UDP Flood 기능을 가지고 있음

## GetHWID
- Server에는 단순한 하드락이 있고 이는 해당 HWID를 얻기 위한 단순한 프로그램

## Server
- Rat의 Server 소스코드
- 최소한의 통신으로 많은 봇들을 수용 가능
- 아주 기본적인 기능만을 가짐
- Console에 기반
