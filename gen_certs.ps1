$OPENSSL = "C:\Program Files\rti_connext_dds-7.5.0\third_party\openssl-3.0.12\x64Win64VS2017\release\bin\openssl.exe"
$env:OPENSSL_CONF = "C:\Program Files\rti_connext_dds-7.5.0\third_party\openssl-3.0.12\x64Win64VS2017\release\openssl.cnf"
$CERT_DIR = "e:\Workspace\rti-msvc-cpp-ex1\certs"

if (-not (Test-Path $CERT_DIR)) {
    New-Item -ItemType Directory -Path $CERT_DIR
}

Set-Location $CERT_DIR

Write-Host "--- Generating Identity CA ---"
& $OPENSSL genrsa -out "$CERT_DIR\identity_ca_key.pem" 2048
& $OPENSSL req -x509 -new -nodes -key "$CERT_DIR\identity_ca_key.pem" -days 3650 -out "$CERT_DIR\identity_ca_cert.pem" -subj "/CN=IdentityCA/O=RTI/C=KR"

Write-Host "--- Generating Permissions CA ---"
& $OPENSSL genrsa -out "$CERT_DIR\permissions_ca_key.pem" 2048
& $OPENSSL req -x509 -new -nodes -key "$CERT_DIR\permissions_ca_key.pem" -days 3650 -out "$CERT_DIR\permissions_ca_cert.pem" -subj "/CN=PermissionsCA/O=RTI/C=KR"

Write-Host "--- Generating App1 Identity ---"
& $OPENSSL genrsa -out "$CERT_DIR\app1_key.pem" 2048
& $OPENSSL req -new -key "$CERT_DIR\app1_key.pem" -out "$CERT_DIR\app1_csr.pem" -subj "/CN=App1/O=RTI/C=KR"
& $OPENSSL x509 -req -in "$CERT_DIR\app1_csr.pem" -CA "$CERT_DIR\identity_ca_cert.pem" -CAkey "$CERT_DIR\identity_ca_key.pem" -CAcreateserial -out "$CERT_DIR\app1_cert.pem" -days 3650

Write-Host "--- Generating App2 Identity ---"
& $OPENSSL genrsa -out "$CERT_DIR\app2_key.pem" 2048
& $OPENSSL req -new -key "$CERT_DIR\app2_key.pem" -out "$CERT_DIR\app2_csr.pem" -subj "/CN=App2/O=RTI/C=KR"
& $OPENSSL x509 -req -in "$CERT_DIR\app2_csr.pem" -CA "$CERT_DIR\identity_ca_cert.pem" -CAkey "$CERT_DIR\identity_ca_key.pem" -CAcreateserial -out "$CERT_DIR\app2_cert.pem" -days 3650

Write-Host "--- Signing Governance.xml ---"
& $OPENSSL smime -sign -in "$CERT_DIR\governance.xml" -text -out "$CERT_DIR\governance.p7s" -signer "$CERT_DIR\permissions_ca_cert.pem" -inkey "$CERT_DIR\permissions_ca_key.pem" -nodetach -outform PEM

Write-Host "--- Signing Permissions.xml ---"
& $OPENSSL smime -sign -in "$CERT_DIR\permissions.xml" -text -out "$CERT_DIR\permissions.p7s" -signer "$CERT_DIR\permissions_ca_cert.pem" -inkey "$CERT_DIR\permissions_ca_key.pem" -nodetach -outform PEM

Write-Host "--- Verification ---"
Get-ChildItem -Path $CERT_DIR
