all: otp_enc.c otp_enc_d.c otp_dec.c otp_dec_d.c keygen.c
	gcc otp_enc.c -o otp_enc -Wall
	gcc otp_enc_d.c -o otp_enc_d -Wall
	gcc otp_dec.c -o otp_dec -Wall
	gcc otp_dec_d.c -o otp_dec_d -Wall
	gcc keygen.c -o keygen -Wall

clean: 
	rm -f otp_enc
	rm -f otp_enc_d
	rm -f otp_dec
	rm -f otp_dec_d 
	rm -f keygen 
	rm -f key20 
	rm -f key70000 
	rm -f cipher* 
	rm -f plaintext*_a 
