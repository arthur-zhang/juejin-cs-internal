package myhack

import (
	"context"
	"github.com/coredns/caddy"
	"github.com/coredns/coredns/core/dnsserver"
	"github.com/coredns/coredns/plugin"
	"github.com/coredns/coredns/request"
	"github.com/miekg/dns"
	"time"
)

type MyHack struct {
	Next plugin.Handler
}

func (e MyHack) Name() string { return "myhack" }

func (e MyHack) ServeDNS(ctx context.Context, w dns.ResponseWriter, r *dns.Msg) (int, error) {
	state := request.Request{W: w, Req: r}
	a := new(dns.Msg)
	a.SetReply(r)
	a.Authoritative = true

	if state.QType() == dns.TypeAAAA && state.Name() == "test.ya.me." {
		a.Rcode = dns.RcodeServerFailure
		time.Sleep(3 * time.Second)
	} else if state.QType() == dns.TypeA && state.Name() == "test.ya.me." {
		time.Sleep(1 * time.Second)
		plugin.NextOrFailure(e.Name(), e.Next, ctx, w, r)
		return dns.RcodeSuccess, nil
	} else {
		plugin.NextOrFailure(e.Name(), e.Next, ctx, w, r)
		return dns.RcodeSuccess, nil
	}

	w.WriteMsg(a)
	return dns.RcodeSuccess, nil
}

func (e MyHack) Ready() bool { return true }

func init() {
	plugin.Register("myhack", func(c *caddy.Controller) error {
		dnsserver.GetConfig(c).AddPlugin(func(next plugin.Handler) plugin.Handler {
			return MyHack{Next: next}
		})
		return nil
	})
}

