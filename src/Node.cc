/* Copyright 2018 Vojtěch Hauser <Vojtech.Hauser+dev@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Distributions of all or part of the Software intended to be used by the
 * recipients as they would use the unmodified Software, containing
 * modifications that substantially alter, remove, or disable functionality of
 * the Software, outside of the documented configuration mechanisms provided by
 * the Software, shall be modified such that the Original Author's bug reporting
 * email addresses and urls are either replaced with the contact information of
 * the parties responsible for the changes, or removed entirely.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "Node.h"

Node::Node(std::vector<std::unique_ptr<Protocol>> protocol,
           std::uint32_t systemId)
        : node(ns3::CreateObject<ns3::Node>(systemId)),
          protocols(std::make_move_iterator(protocol.begin()),
                    std::make_move_iterator(protocol.end())),
          device(ns3::CreateObject<ns3::LrWpanNetDevice>())
{
    node->AddDevice(device);
}

void Node::set_position(const ns3::Vector3D position)
{
    auto mobility_model =
            ns3::CreateObject<ns3::ConstantPositionMobilityModel>();
    mobility_model->SetPosition(position);

    device->GetPhy()->SetMobility(mobility_model);
}

void Node::set_channel(ns3::Ptr<ns3::SingleModelSpectrumChannel> channel)
{
    device->SetChannel(channel);
}

void Node::set_address(const std::optional<std::string> address)
{
    const auto mac_layer = device->GetMac();
    const auto mac16 = address ? ns3::Mac16Address(address->c_str())
                               : ns3::Mac16Address::Allocate();

    mac_layer->SetShortAddress(mac16);
}

void Node::set_pan(const std::optional<std::uint16_t> pan_id)
{
    const auto mac_layer = device->GetMac();
    const std::uint16_t mac16 = pan_id.value_or(0);

    mac_layer->SetPanId(pan_id.value_or(0));
    mac_layer->SetAssociationStatus(ns3::ASSOCIATED);
}

void Node::init_protocols()
{
    auto mac = device->GetMac();

    ns3::McpsDataConfirmCallback conf_cb;
    conf_cb = ns3::MakeCallback(&Node::on_MCPS_DATA_confirm, this);
    mac->SetMcpsDataConfirmCallback(conf_cb);

    ns3::McpsDataIndicationCallback ind_cb;
    ind_cb = ns3::MakeCallback(&Node::on_MCPS_DATA_indication, this);
    mac->SetMcpsDataIndicationCallback(ind_cb);

    for (auto &protocol : protocols)
    {
        protocol->init(this);
    }
}

void
Node::send_packet(std::vector<std::uint8_t> data, ns3::Mac16Address destination,
                  ns3::Time delay, bool ack)
{
    auto pkt = ns3::Create<ns3::Packet>(data.data(), data.size());

    //auto header = ns3::CreateObject<ns3::LrWpanMacHeader>();

    ns3::McpsDataRequestParams params;
    params.m_dstPanId = device->GetMac()->GetPanId();
    params.m_srcAddrMode = ns3::SHORT_ADDR;
    params.m_dstAddrMode = ns3::SHORT_ADDR;
    params.m_dstAddr = destination;
    params.m_msduHandle = 0;
    params.m_txOptions = ack ? ns3::TX_OPTION_ACK : ns3::TX_OPTION_NONE;

    ns3::Simulator::Schedule(delay, &ns3::LrWpanMac::McpsDataRequest,
                             device->GetMac(), params, pkt);
}

constexpr bool Node::is_sink()
{
    return false;
}

void
Node::enable_pcap(const std::string &prefix, bool promiscuous,
                  bool explicit_filename)
{
    helper.EnablePcap(prefix, device, promiscuous, explicit_filename);
}

void Node::enable_ascii(const std::string &prefix, bool explicit_filename)
{
    helper.EnableAscii(prefix, device, explicit_filename);
}

std::ostream &operator<<(std::ostream &os, const Node &node)
{
    os << Node::get_type()
       << " {"
       << "address: " << node.device->GetMac()->GetShortAddress();

    if (node.device->GetPhy()->GetMobility())
    {
        os << ", position: "
           << node.device->GetPhy()->GetMobility()->GetPosition();
    }

    os << "}";

    return os;
}

void Node::on_MCPS_DATA_indication(ns3::McpsDataIndicationParams params,
                                   ns3::Ptr<ns3::Packet> packet)
{
    for (auto &protocol : protocols)
    {
        protocol->on_rx(this, packet);
    }
}

void Node::on_MCPS_DATA_confirm(ns3::McpsDataConfirmParams params)
{
    std::optional<bool> ack_ok;
    switch (params.m_status)
    {
        case ns3::IEEE_802_15_4_SUCCESS:
        {
            ack_ok = true;
            break;
        }
        case ns3::IEEE_802_15_4_NO_ACK:
        {
            ack_ok = false;
            break;
        }
        default:
            break;
    }

    for (auto &protocol : protocols)
    {
        protocol->on_tx(this, ack_ok);
    }
}

ns3::LrWpanHelper Node::helper;

