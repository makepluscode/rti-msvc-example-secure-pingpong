#include <chrono>
#include <dds/dds.hpp>
#include <iostream>
#include <memory>
#include <thread>

#include "ping_pong.hpp"


using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::sub;
using namespace dds::core;

/**
 * @brief App1 클래스: Ping 메시지를 발행하고 Pong 메시지를 구독함
 * 비동기 콜백(Listener)을 사용하여 데이터를 처리합니다.
 */
class PingApp {
   public:
    class PongListener : public dds::sub::NoOpDataReaderListener<PingPongMessage> {
       public:
        void on_data_available(DataReader<PingPongMessage>& reader) override {
            // 읽기 가능 상태인 모든 샘플을 가져옴
            auto samples = reader.take();
            for (const auto& sample : samples) {
                if (sample.info().valid()) {
                    const auto& data = sample.data();
                    std::cout << "[app1] (Async) Pong 수신 (sender: " << data.sender_id()
                              << ", seq: " << data.sequence_num() << ")" << std::endl;
                }
            }
        }
    };

    PingApp(int domain_id)
        : participant_(domain_id),  // Default QosProfile 사용 (보안 미적용)
          publisher_(participant_),
          subscriber_(participant_),
          ping_topic_(participant_, "Ping"),
          pong_topic_(participant_, "Pong"),
          writer_(publisher_, ping_topic_),
          reader_(subscriber_, pong_topic_) {
        // [보안 활성 방법] 위 participant_ 생성 시 아래 주석 코드로 교체:
        // participant_(domain_id,
        // dds::core::QosProvider::Default().participant_qos("Security_Library::Secure_PingPong_Profile"))

        // Listener 설정
        listener_ = std::make_shared<PongListener>();
        reader_.listener(listener_.get(), dds::core::status::StatusMask::data_available());
    }

    void run() {
        std::cout << "--- [app1] 비동기 콜백 버전 시작 ---" << std::endl;
        int count = 1;
        while (true) {
            send_ping(count++);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

   private:
    void send_ping(int seq) {
        PingPongMessage msg("app1", seq);
        std::cout << "[app1] Ping 전송 (seq: " << seq << ")" << std::endl;
        writer_.write(msg);
    }

    DomainParticipant participant_;
    Publisher publisher_;
    Subscriber subscriber_;
    Topic<PingPongMessage> ping_topic_;
    Topic<PingPongMessage> pong_topic_;
    DataWriter<PingPongMessage> writer_;
    DataReader<PingPongMessage> reader_;
    std::shared_ptr<PongListener> listener_;
};

int main() {
    try {
        PingApp app(0);
        app.run();
    } catch (const std::exception& ex) {
        std::cerr << "에러 발생: " << ex.what() << std::endl;
        return -1;
    }
    return 0;
}
