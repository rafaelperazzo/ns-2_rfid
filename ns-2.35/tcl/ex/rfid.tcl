# Define options
set val(chan) Channel/WirelessChannel ;# channel type
set val(prop) Propagation/TwoRayGround ;# radio-propagation model
set val(netif) Phy/WirelessPhy ;# network interface type
set val(mac) Mac/802_11 ;# MAC type
set val(ifq) Queue/DropTail/PriQueue ;# interface queue type
set val(ll) LL ;# link layer type
set val(ant) Antenna/OmniAntenna ;# antenna model
set val(ifqlen) 50 ;# max packet in ifq
set val(nn) 3 ;# number of mobilenodes
set val(rp) DumbAgent ;# routing protocol
set val(x) 1000 ;# X dimension of topography
set val(y) 1000 ;# Y dimension of topography 
set val(stop) 150 ;# time of simulation end

set ns [new Simulator]
set tracefd [open simple.tr w]
set windowVsTime2 [open wintr w]
set namtrace [open rfid2.nam w] 

$ns trace-all $tracefd
$ns namtrace-all-wireless $namtrace $val(x) $val(y)

# set up topography object
set topo [new Topography]

$topo load_flatgrid $val(x) $val(y)

create-god $val(nn)

set chan_1_ [new $val(chan)]

# configure the nodes
$ns node-config -adhocRouting $val(rp) \
-llType $val(ll) \
-macType $val(mac) \
-ifqType $val(ifq) \
-ifqLen $val(ifqlen) \
-antType $val(ant) \
-propType $val(prop) \
-phyType $val(netif) \
-channelType $val(chan) \
-topoInstance $topo \
-agentTrace ON \
-routerTrace ON \
-macTrace OFF \
-movementTrace ON

for {set i 0} {$i < $val(nn) } { incr i } {
	set n($i) [$ns node] 
}

# Provide initial location of mobilenodes..
$n(0) set X_ 80
$n(0) set Y_ 200
$n(0) set Z_ 0.0

$n(1) set X_ 150
$n(1) set Y_ 110
$n(1) set Z_ 0.0

$n(2) set X_ 180
$n(2) set Y_ 110
$n(2) set Z_ 0.0

#defining heads
$ns at 0.0 "$n(0) label LEITOR"
$ns at 0.0 "$n(1) label TAG"
$ns at 0.0 "$n(2) label TAG"
#$ns at 0.0 "$n(2) label TAG"

#Definindo conexão
set reader1 [new Agent/RfidReader]
set tag1 [new Agent/RfidTag]
set tag2 [new Agent/RfidTag]

#Definindo parametros dos agentes
$reader1 set id_ 200
$tag1 set tagEPC_ 11
$tag2 set tagEPC_ 12

$ns attach-agent $n(0) $reader1
$ns attach-agent $n(1) $tag1
$ns attach-agent $n(2) $tag2

#conectando nos
$ns connect $reader1 $tag1
$ns connect $reader1 $tag2

$ns at 1.0 "$reader1 query-tags"

# Define node initial position in nam
$ns initial_node_pos $n(0) 60
for {set i 1} {$i < $val(nn)} { incr i } {
	# 30 defines the node size for nam
	$ns initial_node_pos $n($i) 30
}

# dynamic destination setting procedure..
#$ns at 0.0 "destination"
proc destination {} {
      global ns val n
      set time 1.0
      set now [$ns now]
      for {set i 1} {$i<$val(nn)} {incr i} {
            set xx [expr rand()*1000]
            set yy [expr rand()*1000]
	    set zz [expr rand()*60]
            $ns at $now "$n($i) setdest $xx $yy $zz"
      }
      $ns at [expr $now+$time] "destination"
}

# Telling nodes when the simulation ends
for {set i 0} {$i < $val(nn) } { incr i } {
$ns at $val(stop) "$n($i) reset";
}

# ending nam and the simulation 
$ns at $val(stop) "$ns nam-end-wireless $val(stop)"
$ns at $val(stop) "stop"
$ns at 150.0 "puts \"end simulation\" ; $ns halt"



proc stop {} {
global ns tracefd namtrace
$ns flush-trace
close $tracefd
close $namtrace
#exec nam simwrls.nam &
}
$ns run
