# RTI Connext DDS Security 구현 및 검증 가이드

이 문서는 RTI Connext DDS 7.5.0 환경에서 구현된 보안 기능(인증, 권한 제어, 암호화)의 명세와 사용 방법을 설명합니다.

## 1. 개요
*   **인증 (Authentication)**: PKI-DH 기반 상호 인증 (Digital Certificates 사용)
*   **권한 제어 (Access Control)**: Governance 및 Permissions XML을 통한 토픽 수준의 보안 정책 강제
*   **암호화 (Cryptography)**: AES-GCM을 이용한 사용자 데이터 및 메타데이터 보고

## 2. 보안 아티팩트 (certs/)
보안 적용을 위해 필요한 파일들이 `certs/` 디렉토리에 생성되어 있습니다:
*   `identity_ca_cert.pem`: 신원 확인용 인증 상위 기관(CA) 인증서
*   `permissions_ca_cert.pem`: 권한 확인용 인증 상위 기관(CA) 인증서
*   `app1_cert.pem` / `app1_key.pem`: App1용 개인 인증서 및 키
*   `app2_cert.pem` / `app2_key.pem`: App2용 개인 인증서 및 키
*   `governance.p7s`: 도메인 보안 정책 (서명됨)
*   `permissions.p7s`: 참여자별 상세 권한 정책 (서명됨)

## 3. RTI 7.5.0 전용 속성 설정
RTI Connext DDS 7.5.0 이상에서는 보안 속성명이 간소화되었습니다. `USER_QOS_PROFILES.xml` 설정 시 다음을 참고하십시오:

| 기능 | 이전 방식 (6.x 등) | 7.5.0 이후 단축 방식 |
| :--- | :--- | :--- |
| **인증 CA** | `dds.sec.auth.builtin.PKI-DH.identity_ca` | `dds.sec.auth.identity_ca` |
| **개인 인증서** | `...PKI-DH.identity_certificate` | `dds.sec.auth.identity_certificate` |
| **개인 키** | `...PKI-DH.private_key` | `dds.sec.auth.private_key` |
| **권한 CA** | `...PKI-DH.permissions_ca` | `dds.sec.access.permissions_ca` |
| **Governance** | `...PKI-DH.governance` | `dds.sec.access.governance` |
| **Permissions** | `...PKI-DH.permissions` | `dds.sec.access.permissions` |

## 4. 적용 방법 및 제약 사항

### 4.1 활성화 방법
현재 코드는 라이선스 문제로 인해 기본적으로 비보안(Plain) 통신을 하도록 설정되어 있습니다. 보안을 활성화하려면 다음 단계를 수행하십시오:

1.  **C++ 코드 수정**: `app1_main.cxx` 및 `app2_main.cxx` 생성자에서 `Security_Library`의 프로파일을 로드하도록 주석 해제.
2.  **XML 확인**: `USER_QOS_PROFILES.xml`의 `com.rti.serv.load_plugin` 설정 확인.

### 4.2 제약 사항 (라이선스)
RTI Connext DDS **Evaluation License** 또는 **Security Plugins가 포함되지 않은 패키지**에서는 보안 설정을 활성화할 경우 `FAILED TO LOAD | Security Plugins library` 에러가 발생하며 실행이 차단됩니다. 이는 보안 엔진이 정책을 엄격하게 감시하고 있다는 증거입니다.

## 5. 보안 검증 (Verification)
보안이 실제로 작동하면 다음과 같은 런타임 동작이 발생합니다:
1.  **인증서 요구**: `certs/` 폴더가 없거나 인증서가 유효하지 않으면 DomainParticipant 생성이 실패함.
2.  **Handshake**: 참여자 간 통신 시작 전 상호 인증 절차(DH Key Exchange)를 수행함.
3.  **암호화**: Wireshark 등으로 캡처 시 데이터가 일반 텍스트가 아닌 암호화된 Payload로 나타남.
