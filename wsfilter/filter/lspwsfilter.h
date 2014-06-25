/*
 *  This file is a part of the PARUS project.
 *  Copyright (C) 2006  Alexey N. Salnikov, Roman V. Kolpakov
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Alexey N. Salnikov (salnikov@cmc.msu.ru)
 * Roman V. Kolpakov (roma123@starnet.ru)
 *
 */

#ifndef __LSPWSFILTER_H__
#define __LSPWSFILTER_H__

#include "aflibFFT.h"

#define PI 3.1415926535897932384626433832795

typedef enum _WSFILTER_TYPE
{
    WSFILTER_LOWPASS  = 0,
    WSFILTER_HIGHPASS,
    WSFILTER_BANDPASS,
    WSFILTER_BANDREJECT
} WSFILTER_TYPE;


/**
 * Frequency filter based on Fast Fourie Translation (FFT) 
 * and Balman windowing
 */
class lspWSFilter
{
private:
    double         *data;
    unsigned long  len;
    double         *data2;
    unsigned long  len2;
    int            sample_rate;
    int            window_size;
    double         frequency;
    double         *convolution_kernel;
    int            filter_type;

    aflibFFT fft;

public:
	/**
	 * Creates kernel for filtering.
	 */
    lspWSFilter(int _SampleRate, double _Freq, int window_size, int filter_type);
    ~lspWSFilter(void);

    int process(double * _data, unsigned long _len);
    int process2(double * _data,  unsigned int _len, double * _data2, unsigned int _len2);
	/**
	 * Process window with length window_size 
	 * @param data_in_length must be lower then window_size 
	 * @return -1 on Error and -2 on incorrect data_in_len
	 */
    int processWindow(double * data_in, unsigned int data_in_len, double * data_out);
};

#endif

