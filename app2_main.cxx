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
 * @brief App2 클래스: Ping 메시지를 구독하고 Pong 메시지로 응답함
 * 비동기 콜백(Listener)을 사용하여 데이터를 처리합니다.
 */
class PongApp {
   public:
    class PingListener : public dds::sub::NoOpDataReaderListener<PingPongMessage> {
       public:
        PingListener(DataWriter<PingPongMessage>& writer) : writer_(writer) {}

        void on_data_available(DataReader<PingPongMessage>& reader) override {
            auto samples = reader.take();
            for (const auto& sample : samples) {
                if (sample.info().valid()) {
                    int seq = sample.data().sequence_num();
                    std::cout << "[app2] (Async) Ping 수신 (seq: " << seq << ")" << std::endl;

                    // 응답으로 Pong 전송
                    PingPongMessage response("app2", seq);
                    writer_.write(response);
                }
            }
        }

        void on_subscription_matched(
            DataReader<PingPongMessage>& reader,
            const dds::core::status::SubscriptionMatchedStatus& status) override {
            std::cout << "[app2] (Callback) Subscription matched: total_count="
                      << status.total_count() << std::endl;
        }

        void on_liveliness_changed(
            DataReader<PingPongMessage>& reader,
            const dds::core::status::LivelinessChangedStatus& status) override {
            std::cout << "[app2] (Callback) Liveliness changed: alive_count="
                      << status.alive_count() << std::endl;
        }

       private:
        DataWriter<PingPongMessage>& writer_;
    };

    PongApp(int domain_id)
        : participant_(domain_id),  // Default QosProfile 사용 (보안 미적용)
          publisher_(participant_),
          subscriber_(participant_),
          ping_topic_(participant_, "Ping"),
          pong_topic_(participant_, "Pong"),
          writer_(publisher_, pong_topic_),
          reader_(subscriber_, ping_topic_) {
        // [보안 활성 방법] 위 participant_ 생성 시 아래 주석 코드로 교체:
        // participant_(domain_id,
        // dds::core::QosProvider::Default().participant_qos("Security_Library::Secure_PingPong_Profile_App2"))

        // Listener 설정
        listener_ = std::make_shared<PingListener>(writer_);
        reader_.listener(listener_.get(),
                         dds::core::status::StatusMask::data_available() |
                             dds::core::status::StatusMask::subscription_matched() |
                             dds::core::status::StatusMask::liveliness_changed());
    }

    void run() {
        std::cout << "--- [app2] 비동기 콜백 버전 시작 (Ping 대기 중) ---" << std::endl;
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

   private:
    DomainParticipant participant_;
    Publisher publisher_;
    Subscriber subscriber_;
    Topic<PingPongMessage> ping_topic_;
    Topic<PingPongMessage> pong_topic_;
    DataWriter<PingPongMessage> writer_;
    DataReader<PingPongMessage> reader_;
    std::shared_ptr<PingListener> listener_;
};

int main() {
    try {
        PongApp app(0);
        app.run();
    } catch (const std::exception& ex) {
        std::cerr << "에러 발생: " << ex.what() << std::endl;
        return -1;
    }
    return 0;
}
