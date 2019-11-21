
#include "gtest/gtest.h"

#include <WireFormatter.h>
#include <ReadBufferMock.h>
#include <WriteBufferMock.h>

#include <cstdint>    
#include <limits> 

// EAMS message definitions
#include <repeated_fields.h>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetArgReferee;

namespace test_EmbeddedAMS_RepeatedFieldMessage
{

static constexpr uint32_t Y_SIZE = 3;

TEST(RepeatedFieldMessage, construction) 
{
  repeated_fields<Y_SIZE> msg;
  repeated_message<Y_SIZE> msg2;
}

TEST(RepeatedFieldMessage, serialize_empty_fields) 
{
  repeated_fields<Y_SIZE> msg;

  Mocks::WriteBufferMock buffer;
  EXPECT_CALL(buffer, push(_)).Times(0);
  EXPECT_CALL(buffer, push(_,_)).Times(0);
  EXPECT_CALL(buffer, get_available_size()).WillRepeatedly(Return(99));

  EXPECT_TRUE(msg.serialize(buffer));

  EXPECT_EQ(0, msg.serialized_size());
}

TEST(RepeatedFieldMessage, serialize_empty_message) 
{
  repeated_message<Y_SIZE> msg;

  Mocks::WriteBufferMock buffer;
  EXPECT_CALL(buffer, push(_)).Times(0);
  EXPECT_CALL(buffer, push(_,_)).Times(0);
  EXPECT_CALL(buffer, get_available_size()).WillRepeatedly(Return(99));

  EXPECT_TRUE(msg.serialize(buffer));

  EXPECT_EQ(0, msg.serialized_size());
}

TEST(RepeatedFieldMessage, serialize_array_zero_fields)
{ 
  InSequence s;
  
  Mocks::WriteBufferMock buffer;
  repeated_fields<Y_SIZE> msg;

  msg.add_y(0);
  msg.add_y(0);
  msg.add_y(0);

  uint8_t expected[] = {0x12, 0x03, 0x00, 0x00, 0x00}; // y

  EXPECT_CALL(buffer, get_available_size()).Times(1).WillOnce(Return(6));

  for(auto e : expected) 
  {
    EXPECT_CALL(buffer, push(e)).Times(1).WillOnce(Return(true));
  }

  EXPECT_TRUE(msg.serialize(buffer));
}

TEST(RepeatedFieldMessage, serialize_array_zero_messages)
{ 
  InSequence s;
  
  Mocks::WriteBufferMock buffer;
  repeated_message<Y_SIZE> msg;

  repeated_nested_message rnm;
  rnm.set_u(0);
  rnm.set_v(0);

  msg.add_y(rnm);
  msg.add_y(rnm);
  msg.add_y(rnm);

  EXPECT_CALL(buffer, get_available_size()).Times(1).WillOnce(Return(6));

  EXPECT_CALL(buffer, get_available_size()).Times(1).WillOnce(Return(2));

  EXPECT_CALL(buffer, push(0x12)).Times(1).WillOnce(Return(true));
  EXPECT_CALL(buffer, push(0x00)).Times(1).WillOnce(Return(true));

  EXPECT_CALL(buffer, get_available_size()).Times(1).WillOnce(Return(2));

  EXPECT_CALL(buffer, push(0x12)).Times(1).WillOnce(Return(true));
  EXPECT_CALL(buffer, push(0x00)).Times(1).WillOnce(Return(true));

  EXPECT_CALL(buffer, get_available_size()).Times(1).WillOnce(Return(2));

  EXPECT_CALL(buffer, push(0x12)).Times(1).WillOnce(Return(true));
  EXPECT_CALL(buffer, push(0x00)).Times(1).WillOnce(Return(true));

  EXPECT_TRUE(msg.serialize(buffer));
}

TEST(RepeatedFieldMessage, serialize_array_zero_one_zero)
{
  InSequence s;
  Mocks::WriteBufferMock buffer;

  repeated_fields<Y_SIZE> msg;
  msg.add_y(0);
  msg.add_y(1);
  msg.add_y(0);

  uint8_t expected[] = {0x12, 0x03, 0x00, 0x01, 0x00}; // y

  EXPECT_CALL(buffer, get_available_size()).Times(1).WillOnce(Return(5));

  for(auto e : expected) 
  {
    EXPECT_CALL(buffer, push(e)).Times(1).WillOnce(Return(true));
  }

  EXPECT_TRUE(msg.serialize(buffer));
}


TEST(RepeatedFieldMessage, serialize_array_one)
{
  InSequence s;
  Mocks::WriteBufferMock buffer;

  repeated_fields<Y_SIZE> msg;
  msg.add_y(1);
  msg.add_y(1);
  msg.add_y(1);

  uint8_t expected[] = {0x12, 0x03, 0x01, 0x01, 0x01}; // y

  EXPECT_CALL(buffer, get_available_size()).Times(1).WillOnce(Return(5));

  for(auto e : expected) 
  {
    EXPECT_CALL(buffer, push(e)).Times(1).WillOnce(Return(true));
  }

  EXPECT_TRUE(msg.serialize(buffer));
}



TEST(RepeatedFieldMessage, serialize_array_max)
{
  InSequence s;
  Mocks::WriteBufferMock buffer;

  repeated_fields<Y_SIZE> msg;
  msg.add_y(std::numeric_limits<uint32_t>::max());
  msg.add_y(std::numeric_limits<uint32_t>::max());
  msg.add_y(std::numeric_limits<uint32_t>::max());

  uint8_t expected[] = {0x12, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x0f}; // y

  EXPECT_CALL(buffer, get_available_size()).Times(1).WillOnce(Return(17));

  for(auto e : expected) 
  {
    EXPECT_CALL(buffer, push(e)).Times(1).WillOnce(Return(true));
  }

  EXPECT_TRUE(msg.serialize(buffer));
}



TEST(RepeatedFieldMessage, serialize_one)
{
  InSequence s;
  Mocks::WriteBufferMock buffer;

  repeated_fields<Y_SIZE> msg;
  msg.set_x(1);
  msg.add_y(1);
  msg.add_y(1);
  msg.add_y(1);
  msg.set_z(1);

  uint8_t expected_x[] = {0x08, 0x01};  // x
  
  for(auto e : expected_x) {
    EXPECT_CALL(buffer, push(e)).Times(1).WillOnce(Return(true));
  }                  
  
  uint8_t expected[] = {0x12, 0x03, 0x01, 0x01, 0x01, // y
                        0x18, 0x01}; // z

  EXPECT_CALL(buffer, get_available_size()).Times(1).WillOnce(Return(9));

  for(auto e : expected) 
  {
    EXPECT_CALL(buffer, push(e)).Times(1).WillOnce(Return(true));
  }

  EXPECT_TRUE(msg.serialize(buffer));
}


TEST(RepeatedFieldMessage, serialize_max)
{
  InSequence s;
  Mocks::WriteBufferMock buffer;

  repeated_fields<Y_SIZE> msg;
  msg.set_x(std::numeric_limits<uint32_t>::max());
  msg.add_y(std::numeric_limits<uint32_t>::max());
  msg.add_y(std::numeric_limits<uint32_t>::max());
  msg.add_y(std::numeric_limits<uint32_t>::max());
  msg.set_z(std::numeric_limits<uint32_t>::max());

  uint8_t expected_x[] = {0x08, 0xff, 0xff, 0xff, 0xff, 0x0f};  // x
  
  for(auto e : expected_x) {
    EXPECT_CALL(buffer, push(e)).Times(1).WillOnce(Return(true));
  }                  
  
  uint8_t expected[] = {0x12, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x0f, // y
                        0x18, 0xff, 0xff, 0xff, 0xff, 0x0f}; // z

  EXPECT_CALL(buffer, get_available_size()).Times(1).WillOnce(Return(17));

  for(auto e : expected) 
  {
    EXPECT_CALL(buffer, push(e)).Times(1).WillOnce(Return(true));
  }

  EXPECT_TRUE(msg.serialize(buffer));
}

TEST(RepeatedFieldMessage, deserialize_empty) 
{
  repeated_fields<Y_SIZE> msg;

  Mocks::ReadBufferMock buffer;
  EXPECT_CALL(buffer, pop(_)).WillRepeatedly(Return(false));
  EXPECT_CALL(buffer, get_size()).WillRepeatedly(Return(0));

  EXPECT_TRUE(msg.deserialize(buffer));

}

TEST(RepeatedFieldMessage, deserialize_one) 
{
  InSequence s;

  repeated_fields<Y_SIZE> msg;
  Mocks::ReadBufferMock buffer;

  uint8_t referee[] = {0x08, 0x01, // x
                       0x12, 0x03, 0x01, 0x01, 0x01, // y
                       0x18, 0x01}; // z 

  for(auto r: referee) 
  {
    EXPECT_CALL(buffer, pop(_)).Times(1).WillOnce(DoAll(SetArgReferee<0>(r), Return(true)));
  }
  EXPECT_CALL(buffer, pop(_)).Times(1).WillOnce(Return(false));

  EXPECT_TRUE(msg.deserialize(buffer));

  EXPECT_EQ(1, msg.get_x());
  EXPECT_EQ(3, msg.get_y().get_length());
  EXPECT_EQ(1, msg.y(0));
  EXPECT_EQ(1, msg.y(1));
  EXPECT_EQ(1, msg.y(2));
  EXPECT_EQ(1, msg.get_z());

}

TEST(RepeatedFieldMessage, deserialize_max) 
{
  InSequence s;

  repeated_fields<Y_SIZE> msg;
  Mocks::ReadBufferMock buffer;

  uint8_t referee[] = {0x08, 0xff, 0xff, 0xff, 0xff, 0x0f,  // x
                       0x12, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x0f, // y
                       0x18, 0xff, 0xff, 0xff, 0xff, 0x0f}; // z

  for(auto r: referee) 
  {
    EXPECT_CALL(buffer, pop(_)).Times(1).WillOnce(DoAll(SetArgReferee<0>(r), Return(true)));
  }
  EXPECT_CALL(buffer, pop(_)).Times(1).WillOnce(Return(false));

  EXPECT_TRUE(msg.deserialize(buffer));

  EXPECT_EQ(std::numeric_limits<uint32_t>::max(), msg.get_x());
  EXPECT_EQ(3, msg.get_y().get_length());
  EXPECT_EQ(std::numeric_limits<uint32_t>::max(), msg.y(0));
  EXPECT_EQ(std::numeric_limits<uint32_t>::max(), msg.y(1));
  EXPECT_EQ(std::numeric_limits<uint32_t>::max(), msg.y(2));
  EXPECT_EQ(std::numeric_limits<uint32_t>::max(), msg.get_z());

}

} // End of namespace test_EmbeddedAMS_RepeatedFieldMessage
