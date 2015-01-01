//
//  SBJEV3Brick.h
//  LEGO Control
//
//  Created by David Giovannini on 11/25/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3InvocationStack.h"
#include "SBJEV3DirectCommand.h"
#include "SBJEV3Connection.h"
#include "SBJEV3DeviceIdentifier.h"

#include <memory>

namespace SBJ
{
namespace EV3
{

class ConnectionFactory;
class ConnectionToken;
enum class PromptBluetoothError : int;
	
/*
 * The brick is the high-level object that represents an EV3.
 * It knows how to respond to connection events and create direct commands.
 */
 
class Brick
{
public:
	using ConnectionChanged = std::function<void(Brick& brick)>;
	using PromptBluetoothErrored =  std::function<void(Brick& brick, PromptBluetoothError error)>;

	Brick(ConnectionFactory& factory, const DeviceIdentifier& identifier = DeviceIdentifier());
	
	~Brick();
	
	ConnectionChanged connectionEvent;
	
	bool isConnected() const;
	
	void promptForBluetooth(PromptBluetoothErrored errored);
	
	void disconnect();

	const DeviceIdentifier& identifier() const
	{
		return _identifier;
	}
	
	Connection::Type connectionType() const
	{
		return _connectionType;
	}
	
	ReplyStatus replyStatus() const
	{
		return _replyStatus;
	}
	
	std::string name()
	{
		return _name;
	}
	
	void setName(const std::string& name);

	template <typename...  Opcodes>
	typename DirectCommand<Opcodes...>::Results directCommand(float timeout, Opcodes... opcodes)
	{
		_replyStatus = ReplyStatus::none;
		DirectCommand<Opcodes...> command(_messageCounter, timeout, opcodes...);
		_messageCounter++;
		Invocation invocation(std::move(command.invocation()));
		InvocationScope invocationScope(_stack, invocation);
		_replyStatus = command.status();
		return command.wait();
	}
	/*
	template <typename  Opcode>
	typename SystemCommand<Opcode>::Results systemCommand(float timeout, Opcode opcode)
	{
		_replyStatus = ReplyStatus::none;
		SystemCommand<Opcode> command(_messageCounter, timeout, opcode);
		_messageCounter++;
		Invocation invocation(std::move(command.invocation()));
		InvocationScope invocationScope(_stack, invocation);
		_replyStatus = command.status();
		return command.wait();
	}
	*/
private:
	DeviceIdentifier _identifier;
	std::string _name;
	InvocationStack _stack;
	Connection::Type _connectionType = Connection::Type::none;
	ReplyStatus _replyStatus = ReplyStatus::none;
	std::unique_ptr<ConnectionToken> _token;
	unsigned short _messageCounter = 0;
	
	void handleConnectionChange(const DeviceIdentifier& updatedIdentifier, std::unique_ptr<Connection>& connection);
};
	
}
}
