#include <dds/dds.hpp>
#include <iostream>
#include <memory>

#include "ping_pong.hpp"

using namespace dds::domain;
using namespace dds::topic;
using namespace dds::pub;
using namespace dds::sub;
using namespace dds::core;
using namespace dds::core::cond;

/**
 * @brief App1 클래스: Ping 발행 및 Pong 수신 담당 (비동기 콜백 방식)
 */
class PingApp {
   public:
    class PongListener : public dds::sub::NoOpDataReaderListener<PingPongMessage> {
       public:
        void on_data_available(DataReader<PingPongMessage>& reader) override {
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
        : participant_(domain_id, dds::core::QosProvider::Default().participant_qos(
                                      "Security_Library::Secure_PingPong_Profile")),
          publisher_(participant_),
          subscriber_(participant_),
          ping_topic_(participant_, "Ping"),
          pong_topic_(participant_, "Pong"),
          writer_(publisher_, ping_topic_),
          reader_(subscriber_, pong_topic_) {
        // Listener 설정
        listener_ = std::make_shared<PongListener>();
        reader_.listener(listener_.get(), dds::core::status::StatusMask::data_available());
    }

    void run() {
        std::cout << "--- [app1] 비동기 콜백 버전 시작 ---" << std::endl;

        int count = 1;
        while (true) {
            send_ping(count++);
            // 더 이상 여기서 wait_for_pong()을 호출하지 않음 (콜백에서 처리)
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

   private:
    void send_ping(int seq) {
        PingPongMessage msg("app1", seq);
        std::cout << "[app1] Ping 전송 (seq: " << seq << ")" << std::endl;
        writer_.write(msg);
    }

    // DDS 엔티티 멤버 변수들
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
