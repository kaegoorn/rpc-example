*small example of rpc with multithreading support

abstractcommand - AbstractCommand and RequestHandler, which is inherited from AbstractCommand, in order to be able to implement the server part and the client library.

abstracttransport - Class for implement transport, i.e udp, tcp or in-memory transports

commandfactory - Class for register commands

commandprocessor - Class for async command execution

protocol - Class for serialization and deserialization command data
