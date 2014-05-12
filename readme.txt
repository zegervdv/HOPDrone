-- besluit !!
-- design vereisten
-- accelerometer => niet enkel handig voor kalman maar ook voor energiebesparing
	voelt de accelerometer een versnelling dan kan de filter worden aangezet (dynamic power management)
-- nlos => lange rise time(zorg voor redundantie en smijt nlos metingen eruit of sla een stap over na het opmerken van nLos en pas dead reckoning toe) 
   ook saturatie resultaten mag je niet zomaar gebruiken
-- kalman filter bank => meerdere parallelle filters ( 1 voor bv stilstaan, 1 voor snel voortbewegen, ...)
		--> residu: vergelijk de te verwachten waarde met wat je uitkomt, is dit sterk verschillend dan geef je aan die parallelle filter een laag gewicht