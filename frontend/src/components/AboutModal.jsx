import { motion } from 'framer-motion';
import { FaTimes } from 'react-icons/fa';

const AboutModal = ({ onClose }) => {
  return (
    <motion.div
      initial={{ opacity: 0 }}
      animate={{ opacity: 1 }}
      exit={{ opacity: 0 }}
      className="fixed inset-0 bg-black bg-opacity-90 z-50 flex items-center justify-center p-4"
      onClick={onClose}
    >
      <div 
        className="relative max-w-4xl w-full glass p-8 rounded-xl"
        onClick={(e) => e.stopPropagation()}
      >
        <button
          className="absolute top-4 right-4 text-white hover:text-accent transition-colors"
          onClick={onClose}
        >
          <FaTimes size={24} />
        </button>

        <div className="space-y-8">
          {/* First Sub-section */}
          <div className="space-y-4">
            <h2 className="text-3xl font-display font-bold text-accent">About RayTracerNG</h2>
            <p className="text-gray-300">
            RayTracerNG is a high-performance, real-time 2D ray tracing application built with C++17 and modern OpenGL. It simulates realistic light behavior and reflections in an interactive scene, allowing users to manipulate light sources and obstacles dynamically.
            </p>
          </div>

          {/* Second Sub-section */}
          <div className="space-y-4">
            <h2 className="text-3xl font-display font-bold text-accent">About Me</h2>
            <p className="text-gray-300">
            I am Nischaya Garg, a Computer Science Engineering student specializing in Artificial Intelligence and Machine Learning (Hons. IBM). This project showcases my expertise in C++17, OpenGL, GLFW, GLM, and ImGui through the development of a real-time 2D ray tracing engine, with a focus on performance, interactivity, and modern graphics programming.
            </p>
          </div>
        </div>
      </div>
    </motion.div>
  );
};

export default AboutModal; 