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
 * @brief App2 클래스: Ping 수신 시 즉시 Pong으로 응답하는 챌린저 (비동기 콜백 방식)
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
                    process_ping(sample.data());
                }
            }
        }

       private:
        void process_ping(const PingPongMessage& incoming) {
            int seq = incoming.sequence_num();
            std::cout << "[app2] (Async) Ping 수신 (seq: " << seq << ")" << std::endl;

            // 즉시 응답 생성
            PingPongMessage response("app2", seq);
            std::cout << "[app2] (Async) Pong 전송 (seq: " << seq << ")" << std::endl;
            writer_.write(response);
        }
        DataWriter<PingPongMessage>& writer_;
    };

    PongApp(int domain_id)
        : participant_(domain_id),
          publisher_(participant_),
          subscriber_(participant_),
          ping_topic_(participant_, "Ping"),
          pong_topic_(participant_, "Pong"),
          writer_(publisher_, pong_topic_),
          reader_(subscriber_, ping_topic_) {
        // Listener 설정
        listener_ = std::make_shared<PingListener>(writer_);
        reader_.listener(listener_.get(), dds::core::status::StatusMask::data_available());
    }

    void run() {
        std::cout << "--- [app2] 비동기 콜백 버전 시작 (Ping 대기 중) ---" << std::endl;

        while (true) {
            // 메인 루프에서는 특별히 할 일 없음 (비동기로 처리됨)
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
