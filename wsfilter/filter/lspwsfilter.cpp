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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lMacros.h"
#include "lspwsfilter.h"

/*******************************************************************************************************/
lspWSFilter::lspWSFilter(int _sample_rate, double _Freq, int _window_size, int _Type)
{
	
	int i;
    
	int window_size2 = _window_size/2;

    double  k  = 2 * PI / (double)_window_size;
    double  k2 = 2 * k;
    double  fk = 2 * PI * _Freq;
	double  fk2;
    double  *convolution_kernel2;
    double  sum = 0, sum2 = 0;


	sample_rate             = _sample_rate;
	filter_type             = _Type;
	frequency               = _Freq;
	window_size             = _window_size;
	data=NULL;
	data2=NULL;

    convolution_kernel      = (double *)malloc(2*window_size*sizeof(double));
	convolution_kernel2		= (double *)malloc(2*window_size*sizeof(double));
	
	for(i = 0; i < window_size*2; i++  )
		convolution_kernel[i]  = 0.0;
	for(i = 0; i < window_size*2; i++  )
		convolution_kernel2[i] = 0.0;
	
	switch(filter_type)
	{
	
		case WSFILTER_LOWPASS :
			
			for(i = 0; i <= window_size; i++)
			{
				/*
				 *
				 * Sinc Function sin(x)/x
				 *
				 */
				
				if(i-window_size2)
				{
					convolution_kernel[i]=sin(fk * (i - window_size2))/(double)(i - window_size2);
				}
				else
				{
					convolution_kernel[i] = fk;
				}
				
				/*
				 *
				 * Blackman Windowing
				 *
				 */
				convolution_kernel[i] *= 0.42 - 0.5*cos((double)i * k) + 0.08*cos((double)i * k2);
			}
			
			for(i = 0; i <= window_size; i++)
			{
				sum += convolution_kernel[i];
			}
			
			for(i = 0; i <= window_size; i++)
			{
				convolution_kernel[i] /= l_ABS(sum);
			}
		
		break;
		case WSFILTER_HIGHPASS :
		
			for(i = 0; i <= window_size; i++)
			{
				/*
				 *
				 * Sinc Function
				 *
				 */
				if(i - window_size2)
				{
					convolution_kernel[i]=sin(fk * (i - window_size2))/(double)(i - window_size2);
				}
				else
				{
					convolution_kernel[i] = fk;
				}
				/*
				 *
				 * Blackman Windowing
				 *
				 */
				convolution_kernel[i] *= (-1)* ( 0.42 - 0.5*cos((double)i * k) + 0.08*cos((double)i * k2));
			}

			for(i = 0; i <= window_size; i++)
			{
				sum += convolution_kernel[i];
			}

			for(i = 0; i <= window_size; i++)
			{
				convolution_kernel[i] /= l_ABS(sum);
			}
			
			convolution_kernel[window_size/2] += 1;
			
		break;		
		case WSFILTER_BANDREJECT :
			fk  = 2 * PI * (frequency-0.0005);
			fk2 = 2 * PI * (frequency+0.0005);
			
			for(i = 0; i <= window_size; i++)
			{
				/*
				 *
				 * Sinc Function for first kernel
				 *
				 */
				if(i - window_size2)
				{
					convolution_kernel[i]=sin(fk * (i - window_size2))/(double)(i - window_size2);
				}
				else
				{
					convolution_kernel[i] = fk;
				}

				/*
				 *
				 * Blackman Windowing for first kernel
				 *
				 */
				convolution_kernel[i] *= 0.42 - 0.5*cos((double)i * k) + 0.08*cos((double)i * k2);

				/*
				 *
				 * Sinc Function for second kernel
				 *
				 */
				if(i - window_size2)
				{
					convolution_kernel2[i]=sin(fk2 * (i - window_size2))/(double)(i - window_size2);
				}
				else
				{
					convolution_kernel2[i] = fk2;
				}

				/*
				 *
				 * Blackman Windowing for second  kernel
				 *
				 */
				convolution_kernel2[i] *= (-1)*(0.42 - 0.5*cos((double)i * k) + 0.08*cos((double)i * k2));
			}
			
			for(i = 0; i <= window_size; i++)
			{
				sum  += convolution_kernel[i];
				sum2 += convolution_kernel2[i];
			}
			
			for(i = 0; i <= window_size; i++)
			{
				convolution_kernel[i]  /= l_ABS(sum);
				convolution_kernel2[i] /= l_ABS(sum2);
			}
			
			convolution_kernel2[window_size/2] += 1;
			
			for(i = 0; i <= window_size; i++)
			{
				convolution_kernel[i] += convolution_kernel2[i];
			}

        break;
		case WSFILTER_BANDPASS :
			fk  = 2 * PI * (frequency-0.0005);
			fk2 = 2 * PI * (frequency+0.0005);
			
			for(i = 0; i <= window_size; i++)
			{
				/*
				 *
				 * Sinc Function for first kernel
				 *
				 */
				if(i - window_size2)
				{
					convolution_kernel[i]=sin(fk * (i - window_size2))/(double)(i - window_size2);
				}
				else
				{
					convolution_kernel[i] = fk;
				}

				/*
				 *
				 * Blackman Windowing for first kernel
				 *
				 */
				convolution_kernel[i] *= 0.42 - 0.5*cos((double)i * k) + 0.08*cos((double)i * k2);

				/*
				 *
				 * Sinc Function for second kernel
				 *
				 */
				if(i - window_size2)
				{
					convolution_kernel2[i]=sin(fk2 * (i - window_size2))/(double)(i - window_size2);
				}
				else
				{
					convolution_kernel2[i] = fk2;
				}

				/*
				 *
				 * Blackman Windowing for second  kernel
				 *
				 */
				convolution_kernel2[i] *= (-1)*(0.42 - 0.5*cos((double)i * k) + 0.08*cos((double)i * k2));
			}
			
			for(i = 0; i <= window_size; i++)
			{
				sum  += convolution_kernel[i];
				sum2 += convolution_kernel2[i];
			}
			
			for(i = 0; i <= window_size; i++)
			{
				convolution_kernel[i]  /= l_ABS(sum);
				convolution_kernel2[i] /= l_ABS(sum2);
			}
			
			convolution_kernel2[window_size/2] += 1;
			
			for(i = 0; i <= window_size; i++)
			{
            	convolution_kernel[i] += convolution_kernel2[i];
			}
			
			for(i = 0; i <= window_size; i++)
			{
				convolution_kernel[i] = -convolution_kernel[i];
			}
			
			convolution_kernel[window_size/2] += 1;
		
		break;
	} /* End switch */

	if(convolution_kernel2!=NULL)
	{
		free(convolution_kernel2);
		convolution_kernel2=NULL;
	}

	return;
}
/**********************************************************************************************/
lspWSFilter::~lspWSFilter(void)
{
    if(convolution_kernel!=NULL)
	{
		free(convolution_kernel);
		convolution_kernel=NULL;
	}

	if(data!=NULL)
	{
		free(data);
		data=NULL;
	}

	if(data2=NULL)
	{
		free(data2);
		data2=NULL;
	}

	return;
}
/*********************************************************************************************/
int lspWSFilter::process(double * _data, unsigned long _len)
{
	double *frequency_re;
    double *frequency_im;
    double *in_data_re;
    double *in_data_im;
    double *out_data_re;
    double *out_data_im;
    double  max_re, max_im, mout_re, mout_im;
    double  fi, tmp, tmp2;
    int i, j;

	frequency_re=(double *)malloc(window_size*2*sizeof(double));
	frequency_im=(double *)malloc(window_size*2*sizeof(double));

	in_data_re=(double *)malloc(window_size*2*sizeof(double));
	in_data_im=(double *)malloc(window_size*2*sizeof(double));

	out_data_re=(double *)malloc(window_size*2*sizeof(double));
	out_data_im=(double *)malloc(window_size*2*sizeof(double));

	data  = (double *)malloc(window_size*2*sizeof(double));
	data2 = (double *)malloc(window_size*2*sizeof(double));

	
	if
	(
		(frequency_re==NULL)||
		(frequency_im==NULL)||
		(in_data_re==NULL)||
		(in_data_im==NULL)||
		(out_data_re==NULL)||
		(out_data_im==NULL)||
		(data==NULL)||
		(data2==NULL)
	)
	{
		return -1;
	}

    max_re = max_im = mout_re = mout_im = 0.0;
    len = len2 = window_size;

    for(i = 0; i < window_size*2; i++)
    {
        data[i]          = 0.0;
        data2[i]         = 0.0;
        frequency_re[i]  = 0.0;
        frequency_im[i]  = 0.0;
        in_data_re[i]    = 0.0;
        in_data_re[i]    = 0.0;
        out_data_re[i]   = 0.0;
        out_data_re[i]   = 0.0;
    }

	/*
	 * Get specter for kernel
	 */
    fft.fft_double(window_size*2, 0, convolution_kernel, NULL, frequency_re, frequency_im);

    for(j = 0; j < _len/window_size; j++)
    {
        memcpy(data, _data+j*window_size, sizeof(double)*window_size);
		
		/*
		 *
		 * get specter for input data
		 *
		 */
        fft.fft_double(window_size*2, 0, data, NULL, out_data_re, out_data_im);

        for(i = 0; i < 2*window_size; i++)
        {
            max_re = (l_ABS(frequency_re[i]) > max_re) ? l_ABS(frequency_re[i]) : max_re;
            max_im = (l_ABS(frequency_im[i]) > max_im) ? l_ABS(frequency_im[i]) : max_im;
        }
        for(i = 0; i < 2*window_size; i++)
        {
            frequency_re[i] /= max_re;
            frequency_im[i] /= max_im;
        }

        for(i = 0; i < 2*window_size; i++)
        {
            tmp = out_data_re[i]*frequency_re[i] - out_data_im[i]*frequency_im[i];
            out_data_im[i] = out_data_re[i]*frequency_im[i] + out_data_im[i]*frequency_re[i];
            out_data_re[i] = tmp;
        }

        fft.fft_double(window_size*2, 1, out_data_re, out_data_im, in_data_re, in_data_im);
		
		for(i = 0; i < window_size; i++)
		{
			in_data_re[i] += data2[i];
		}
		
		memcpy(_data + j*window_size, in_data_re, sizeof(double)*window_size);
        memcpy(data2, in_data_re + window_size, sizeof(double)*window_size);
    }

    tmp2 = _len % window_size;
    
	if(tmp2)
    {
        memcpy(data, _data+j*window_size, sizeof(double)*(size_t)tmp2);
        fft.fft_double(window_size*2, 0, data, NULL, out_data_re, out_data_im);

        for(i = 0; i < 2*window_size; i++)
        {
            max_re = (l_ABS(frequency_re[i]) > max_re) ? l_ABS(frequency_re[i]) : max_re;
            max_im = (l_ABS(frequency_im[i]) > max_im) ? l_ABS(frequency_im[i]) : max_im;
        }
        for(i = 0; i < 2*window_size; i++)
        {
            frequency_re[i] /= max_re;
            frequency_im[i] /= max_im;
        }
        for(i = 0; i < 2*window_size; i++)
        {
            tmp = out_data_re[i]*frequency_re[i] - out_data_im[i]*frequency_im[i];
            out_data_im[i] = out_data_re[i]*frequency_im[i] + out_data_im[i]*frequency_re[i];
            out_data_re[i] = tmp;
        }

        fft.fft_double(window_size*2, 1, out_data_re, out_data_im, in_data_re, in_data_im);
        for(i = 0; i < window_size; i++)
		{
            in_data_re[i] += data2[i];
		}

        memcpy(_data + j*window_size, in_data_re, sizeof(double)*(size_t)tmp2);
        memcpy(data2, in_data_re + window_size, sizeof(double)*window_size);
    }
	
	free(frequency_re);
	free(frequency_im);
	free(in_data_re);
	free(in_data_im);
	free(out_data_re);
	free(out_data_im);

	return 0;
}
/********************************************************************************************************/
int lspWSFilter::processWindow(double * data_in, unsigned int data_in_len, double * data_out)
{
    unsigned int window_sizeM2 = window_size*2;
    register unsigned int i;

	double *frequency_re;
    double *frequency_im;
    double *in_data_re;
    double *in_data_im;
    double *out_data_re;
    double *out_data_im;
    
    if(data_in_len > window_size)
        return -2;

    double  max_re = 0;
    double  max_im = 0;
    double  tmp;

	frequency_re=(double *)malloc(window_size*2*sizeof(double));
	frequency_im=(double *)malloc(window_size*2*sizeof(double));

	in_data_re=(double *)malloc(window_size*2*sizeof(double));
	in_data_im=(double *)malloc(window_size*2*sizeof(double));

	out_data_re=(double *)malloc(window_size*2*sizeof(double));
	out_data_im=(double *)malloc(window_size*2*sizeof(double));

	if
	(
		(frequency_re==NULL)||
		(frequency_im==NULL)||
		(in_data_re==NULL)||
		(in_data_im==NULL)||
		(out_data_re==NULL)||
		(out_data_im==NULL)
	)
	{
		return -1;
	}



	for(i = 0; i < window_sizeM2; i++)
    {
        frequency_re[i]       = 0.0;
        frequency_im[i]       = 0.0;
        in_data_re[i]         = 0.0;
        in_data_re[i]         = 0.0;
        out_data_re[i]        = 0.0;
        out_data_re[i]        = 0.0;
    }
    
	// FFT
    fft.fft_double(window_sizeM2, 0, convolution_kernel,   NULL, frequency_re, frequency_im);
    fft.fft_double(window_sizeM2, 0, data_in, NULL, out_data_re, out_data_im);
    
	// Normalizing Frequency Response	
    for(i = 0; i < window_sizeM2; i++)
    {
        max_re = (l_ABS(frequency_re[i]) > max_re) ? l_ABS(frequency_re[i]) : max_re;
        max_im = (l_ABS(frequency_im[i]) > max_im) ? l_ABS(frequency_im[i]) : max_im;
    }
    
	for(i = 0; i < window_sizeM2; i++)
    {
        frequency_re[i] /= max_re;
        frequency_im[i] /= max_im;
    }
    
	// Filtering
    for(i = 0; i < window_sizeM2; i++)
    {
        tmp = out_data_re[i]*frequency_re[i] - out_data_im[i]*frequency_im[i];
        out_data_im[i] = out_data_re[i]*frequency_im[i] + out_data_im[i]*frequency_re[i];
        out_data_re[i] = tmp;
    }
    
	// iFFT
    fft.fft_double(window_sizeM2, 1, out_data_re, out_data_im, in_data_re, in_data_im);
    memcpy(data_out, in_data_re, sizeof(double)*(data_in_len+window_size));
	
	free(frequency_re);
	free(frequency_im);
	free(in_data_re);
	free(in_data_im);
	free(out_data_re);
	free(out_data_im);

    return 0;
}
/**************************************************************************************************/
int lspWSFilter::process2(double * _data, unsigned int _len, double * _data2, unsigned int _len2)
{
	int i, j;
	double *frequency_re;
    double *frequency_im;
    double *in_data_re;
    double *in_data_im;
    double *out_data_re;
    double *out_data_im;
    double  max_re, max_im, mout_re, mout_im;
    double  fi, tmp, tmp2;
   

	frequency_re=(double *)malloc(window_size*2*sizeof(double));
	frequency_im=(double *)malloc(window_size*2*sizeof(double));

	in_data_re=(double *)malloc(window_size*2*sizeof(double));
	in_data_im=(double *)malloc(window_size*2*sizeof(double));

	out_data_re=(double *)malloc(window_size*2*sizeof(double));
	out_data_im=(double *)malloc(window_size*2*sizeof(double));

	data  = (double *)malloc(window_size*2*sizeof(double));
	data2 = (double *)malloc(window_size*2*sizeof(double));

	
	if
	(
		(frequency_re==NULL)||
		(frequency_im==NULL)||
		(in_data_re==NULL)||
		(in_data_im==NULL)||
		(out_data_re==NULL)||
		(out_data_im==NULL)||
		(data==NULL)||
		(data2==NULL)
	)
	{
		return -1;
	}

    max_re = max_im = mout_re = mout_im = 0.0;
    len = len2 = window_size;
	
	
    for(i = 0; i < window_size*2; i++)
    {
        data[i]               = 0.0;
        data2[i]              = 0.0;
        frequency_re[i]       = 0.0;
        frequency_im[i]       = 0.0;
        in_data_re[i]         = 0.0;
        in_data_re[i]         = 0.0;
        out_data_re[i]        = 0.0;
        out_data_re[i]        = 0.0;
    }

    fft.fft_double(window_size*2, 0, convolution_kernel, NULL, frequency_re, frequency_im);
    for(j = 0; j < _len/window_size; j++)
    {
        memcpy(data, _data+j*window_size, sizeof(double)*window_size);
        fft.fft_double(window_size*2, 0, data, NULL, out_data_re, out_data_im);

        for(i = 0; i < 2*window_size; i++)
        {
            max_re = (l_ABS(frequency_re[i]) > max_re) ? l_ABS(frequency_re[i]) : max_re;
            max_im = (l_ABS(frequency_im[i]) > max_im) ? l_ABS(frequency_im[i]) : max_im;
        }
        for(i = 0; i < 2*window_size; i++)
        {
            frequency_re[i] /= max_re;
            frequency_im[i] /= max_im;
        }

        for(i = 0; i < 2*window_size; i++)
        {
            tmp = out_data_re[i]*frequency_re[i] - out_data_im[i]*frequency_im[i];
            out_data_im[i] = out_data_re[i]*frequency_im[i] + out_data_im[i]*frequency_re[i];
            out_data_re[i] = tmp;
        }
        fft.fft_double(window_size*2, 1, out_data_re, out_data_im, in_data_re, in_data_im);
        for(i = 0; i < window_size; i++)
            in_data_re[i] += data2[i];
        memcpy(_data2 + j*window_size, in_data_re, sizeof(double)*window_size);
        memcpy(data2, in_data_re + window_size, sizeof(double)*window_size);
    }

    memcpy(_data2+_len, data2, sizeof(double)*window_size);
    tmp2 = _len % window_size;
    if(tmp2)
    {
        memset(data, 0, sizeof(double)*window_size*2);
        memcpy(data, _data+j*window_size, sizeof(double)*(size_t)tmp2);
        fft.fft_double(window_size*2, 0, data, NULL, out_data_re, out_data_im);

        for(i = 0; i < 2*window_size; i++)
        {
            max_re = (l_ABS(frequency_re[i]) > max_re) ? l_ABS(frequency_re[i]) : max_re;
            max_im = (l_ABS(frequency_im[i]) > max_im) ? l_ABS(frequency_im[i]) : max_im;
        }
        for(i = 0; i < 2*window_size; i++)
        {
            frequency_re[i] /= max_re;
            frequency_im[i] /= max_im;
        }
        for(i = 0; i < 2*window_size; i++)
        {
            tmp = out_data_re[i]*frequency_re[i] - out_data_im[i]*frequency_im[i];
            out_data_im[i] = out_data_re[i]*frequency_im[i] + out_data_im[i]*frequency_re[i];
            out_data_re[i] = tmp;
        }
        fft.fft_double(window_size*2, 1, out_data_re, out_data_im, in_data_re, in_data_im);
        for(i = 0; i < window_size; i++)
            in_data_re[i] += data2[i];
        memcpy(_data2 + j*window_size, in_data_re, sizeof(double)*(window_size+(size_t)tmp2));
        //    memset(data2, 0, sizeof(double)*window_size*2);
        //    memcpy(data2, in_data_re + window_size, sizeof(double)*tmp2);
    }
    //  tmp2 = (tmp2) ? tmp2 : window_size;
	
	free(frequency_re);
	free(frequency_im);
	free(in_data_re);
	free(in_data_im);
	free(out_data_re);
	free(out_data_im);
	free(data);
	data=NULL;
	free(data2);
	data2=NULL;

	return 0;
}

