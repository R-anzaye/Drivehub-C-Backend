/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx,vue}",
  ],
  theme: {
    extend: {
      fontFamily: {
        customFont: ['"Custom Font"', "-apple-system", "BlinkMacSystemFont", "Segoe UI", "Roboto", "Oxygen", "Ubuntu", "Cantarell", "Fira Sans", "Droid Sans", "Helvetica Neue", "sans-serif"],
        sans: ['Poppins', 'ui-sans-serif', 'system-ui'],
        serif: [
          'Georgia',         
          'Times New Roman', 
          ,],
        mono: ['Fira Code', 'monospace'],
        animation: {
          'checkmark-fade': 'checkmark-fade 0.5s ease-out',
        },
      },
      keyframes: {
        'checkmark-fade': {
          '0%': { transform: 'scale(0)', opacity: '0' },
          '50%': { transform: 'scale(1.2)', opacity: '1' },
          '100%': { transform: 'scale(1)', opacity: '1' },
        },
      },
    },
  },
  plugins: [],
}



