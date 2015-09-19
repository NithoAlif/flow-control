#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using boost::asio::ip::udp;

enum { max_length = 1024 };

int main(int argc, char const *argv[]){
	try{
		if (argc < 4){
			cerr << "Usage: transmitter <host> <port> <file>" << endl;
			return 1;
		}
		io_service io_service_;
		udp::socket socket_(io_service_, udp::endpoint(udp::v4(), 0));
		udp::resolver resolver_(io_service_);
		udp::endpoint transmitter_endpoint_ = *resolver_.resolve({udp::v4(), argv[1], argv[2]});

			cout << "Enter message: ";
			char request_[max_length];
			cin.getline(request_, max_length);
			size_t request_length_ = strlen(request_);
			socket_.send_to(buffer(request_, request_length_), transmitter_endpoint_);

		char reply_[max_length];
		udp::endpoint receiver_endpoint_;
		size_t reply_length_ = socket_.receive_from(
		buffer(reply_, max_length), receiver_endpoint_);
		cout << "Reply is: ";
		cout.write(reply_, reply_length_);
		cout << "\n";


	} catch(std::exception& e){
		cerr << "Exception: " << e.what() << endl;
	}
	return 0;
}