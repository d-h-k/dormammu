# dormammu
c++ http

## 주의사항
- 이 리포는 cpp 언어로 TCP/IP 소켓통신을 해서 Oauth 인증을 구현하는 매우 뻘짓을 다룹니다. 바쁘신 분들은 

## 개발환경 구축
- 이 프로젝트를 따라하기 위해서는 2가지 셋팅이 필요합니다. 컴파일러 & 빌드시스템
- 다른언어들과 다르게 cpp 사용시 컴파일러 & 빌드시스템 은 OS 에 따라 많이 다릅니다. 본인 환경에 맞춰서 잘되는걸 쓰시면 됩니다!
- mac : 컴파일러는 CLang이 기본으로 설치되어있어서 빌드시스템만 셋업 : `brew install cmake`
- Linux(Ubuntu)
  ```
  sudo apt update && sudo apt install build-essential gcc make cmake -y
  ```
- Windows 환경의 경우
  ```
  - 컴파일러 : MinGW 기반 GCC 바이너리 설치 > https://gcc.gnu.org/install/binaries.html
  - cmake : 여기서 다운받아 설치 > https://cmake.org/download/
  - 다른방법도 많습니다. 가장 편한걸 쓰시면 됩니다. (다른 IDE 혹은, Linux Subsystem 등등..) 
  ```
- 이외 개발환경 셋업시 참고
  - https://code.visualstudio.com/docs/cpp/CMake-linux
  - https://fascination-euna.tistory.com/entry/P4C-W4-W5-C%EC%96%B8%EC%96%B4%EB%A1%9C-HTTP-%EC%84%9C%EB%B2%84-%EA%B5%AC%ED%98%84
  

