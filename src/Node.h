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

#ifndef MADV_NODE_H
#define MADV_NODE_H

#include <cstdint>
#include <optional>
#include <initializer_list>
#include <memory>

#include <ns3/node.h>
#include <ns3/vector.h>
#include <ns3/mobility-module.h>
#include <ns3/mac16-address.h>
#include <ns3/lr-wpan-module.h>
#include <ostream>
#include <ns3/spectrum-module.h>
#include "Protocol.h"

class Protocol;

class Node
{
public:
    Node(std::vector<std::unique_ptr<Protocol>> protocol,
            std::uint32_t systemId = 0);

    // PHY
    void set_position(ns3::Vector3D position);

    void set_channel(ns3::Ptr<ns3::SingleModelSpectrumChannel> channel);

    // MAC
    void set_address(std::optional<std::string> address = std::nullopt);

    void set_pan(std::optional<std::uint16_t> pan_id = std::nullopt);

    // NWK +
    void init_protocols();

    void send_packet(std::vector<uint8_t> data = {},
                     ns3::Mac16Address destination = ns3::Mac16Address("ff:ff"),
                     ns3::Time delay = ns3::Seconds(0.0),
                     bool ack = false);

    // Non-OSI networking
    constexpr static bool is_sink();

    // Simulation-related
    void enable_pcap(const std::string &prefix, bool promiscuous = false,
                     bool explicit_filename = false);

    void
    enable_ascii(const std::string &prefix, bool explicit_filename = false);

    // Misc
    friend std::ostream &operator<<(std::ostream &os, const Node &node);

protected:
    static std::string get_type()
    {
        return "Node";
    }

private:
    inline void on_MCPS_DATA_indication(ns3::McpsDataIndicationParams params,
                                        ns3::Ptr<ns3::Packet> packet);

    inline void on_MCPS_DATA_confirm(ns3::McpsDataConfirmParams params);

    static ns3::LrWpanHelper helper;

    std::vector<std::unique_ptr<Protocol>> protocols;

    ns3::Ptr<ns3::Node> node;
    ns3::Ptr<ns3::LrWpanNetDevice> device;
};

#endif //MADV_NODE_H
