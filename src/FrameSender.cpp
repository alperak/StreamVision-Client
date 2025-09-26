#include "FrameSender.hpp"

void FrameSender::start() 
{
    if (isRunning_) { // Prevent multiple threads
        return; 
    }
    isRunning_ = true;
    senderThread_ = std::thread(&FrameSender::sendFrameAndReceiveJsonMetadata, this);
}

void FrameSender::stop() 
{
    isRunning_ = false;
    // socket close eklemeli miyim? yoksa zeromq handle ediyor mu?
    if (senderThread_.joinable()) {
        senderThread_.join();
    }
}

void FrameSender::pushEncodedFrame(const std::vector<uchar>&& encodedBuffer)
{
    std::lock_guard<std::mutex> lock(bufferMutex_);
    latestBuffer_ = std::move(encodedBuffer);
}

std::string FrameSender::getLatestJsonMetadata()
{
    std::lock_guard<std::mutex> lock(jsonMutex_);
    return latestJsonMetadata_;
}

void FrameSender::sendFrameAndReceiveJsonMetadata()
{
    while (isRunning_) {
        std::vector<uchar> bufCopy;

        { // burda bir kontrol mekanizması lazım gibi sürekli ne olursa olsun bufcopy'e yazmak mantıklı değil gibi.
            std::lock_guard<std::mutex> lock(bufferMutex_);
            // latestBuffer_ doğrudan kullanılmıyor çünkü send/recv işlemleri yavaş olabilir.
            // Eğer bu işlemleri latestBuffer_ üzerinde kilit açıkken yapsaydık,
            // pushEncodedFrame() fonksiyonu kilidi beklemek zorunda kalır ve frame kaybı / gecikme olurdu.
            // Bu yüzden burada sadece hızlı bir move yapıyoruz (bufCopy <= latestBuffer_),
            // ardından kilidi serbest bırakıp uzun sürebilecek send/recv işlemini kilitsiz yapıyoruz.
           bufCopy = std::move(latestBuffer_);
        }
        
        if (!bufCopy.empty()) { //send ve recive exception handling lazım
            clientSocket_.send(zmq::buffer(bufCopy), zmq::send_flags::none);

            zmq::message_t reply;
            clientSocket_.recv(reply, zmq::recv_flags::none);
            std::string replyStr(static_cast<char*>(reply.data()), reply.size());
            //std::cout << "Server cevabı: " << replyStr << "\n";

            {
                std::lock_guard<std::mutex> lock(jsonMutex_);
                latestJsonMetadata_ = std::move(replyStr);
            }
        }
    }
}