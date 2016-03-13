all:
	gcc otp_enc_d.c -o otp_enc_d -Wall
	gcc otp_enc.c -o otp_enc     -Wall
	gcc otp_dec_d.c -o otp_dec_d -Wall
	gcc otp_dec.c -o otp_dec     -Wall
	gcc keygen.c -o keygen -Wall
clean:
	rm otp_enc otp_enc_d otp_dec otp_dec_d keygen
