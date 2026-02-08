# RTI Connext DDS Ping-Pong Example (C++17)

이 프로젝트는 **RTI Connext DDS 7.5.0**을 사용하여 두 애플리케이션 간에 Ping-Pong 메시지를 주고받는 C++17 기반 예제입니다.

## 주요 특징
- **Modern C++ API**: RTI Connext DDS의 최신 C++11/17 API 사용.
- **Async Callbacks**: `DataReaderListener`를 활용한 이벤트 기반 비동기 데이터 수신 처리.
- **Robustness**: MSVC `/analyze` 및 `/W4` 린트 적용으로 코드 품질 확보.
- **Architecture**: 클래스 기반 리팩토링 및 RAII 패턴 적용.

## 기술 스택
- **언어**: C++17
- **DDS**: RTI Connext DDS 7.5.0
- **컴파일러**: MSVC (Visual Studio 2022 권장)
- **빌드 시스템**: CMake 3.15+

## 프로젝트 구조
- `ping_pong.idl`: 데이터 구조 정의.
- `app1_main.cxx`: Ping 발행 및 Pong 구독자.
- `app2_main.cxx`: Ping 구독 및 Pong 발행자.
- `ARCHITECTURE.md`: 시퀀스 다이어그램 및 상세 아키텍처 설명.
- `USER_QOS_PROFILES.xml`: DDS QoS 설정 (Reliable 통신).

## 빌드 방법
`build.sh` 또는 직접 CMake 명령을 사용하여 빌드할 수 있습니다.

```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

## 실행 방법
라이선스 파일 경로(`C:\Program Files\rti_connext_dds-7.5.0\rti_license.dat`)가 올바른지 확인한 후 실행합니다.

```powershell
./run.ps1
```

## 상세 설계
더 자세한 내용은 [ARCHITECTURE.md](./ARCHITECTURE.md)를 참조하세요.
